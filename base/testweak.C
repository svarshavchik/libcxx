/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "mutex.H"
#include "threads/run.H"
#include "weakptr.H"
#include <iostream>
#include <condition_variable>

class testclass : virtual public LIBCXX_NAMESPACE::obj {

public:
	testclass()
	{
		std::cout << "Constructor" << std::endl;
	}

	~testclass() noexcept
	{
		std::cout << "Destructor" << std::endl;
	}

};

typedef LIBCXX_NAMESPACE::ptr<testclass> testref;
static bool weaktest_go=false;

class weaktestinfo : virtual public LIBCXX_NAMESPACE::obj {

public:
	std::mutex mutex;
	std::condition_variable cond;

	testref strongref;

	volatile bool done;

	void run();

	weaktestinfo() : done(false)
	{
	}

	~weaktestinfo() noexcept
	{
	}
};

void weaktestinfo::run()
{
	LIBCXX_NAMESPACE::weakptr<testref> weakptr(strongref);

	std::cout << "Weak reference acquired"
		  << std::endl << std::flush;

	weaktest_go=true;
	{
		std::unique_lock<std::mutex> lock_test_mutex(mutex);

		std::cout << "Lock acquired in the thread, signaling parent"
			  << std::endl << std::flush;

		cond.notify_one();

		cond.wait(lock_test_mutex);

		std::cout << "Acquiring strong reference..."
			  << std::endl << std::flush;
	}

	testref strongref=weakptr.getptr();

	std::cout << "Acquired strong reference: "
		  << (strongref.null() ? "no":"yes")
		  << std::endl << std::flush;
}

static weaktestinfo *weaktestinfo_ptr;

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::weakptr<testref> weakptr;

	std::cout << "weak ref null flag: "
		  << (weakptr.getptr().null() ? "null":"not null")
		  << std::endl;

	{
		testref strongref=testref::create();

		weakptr=strongref;

		std::cout << "Created weakref" << std::endl;

		{
			testref strongref2=strongref;
		}

		std::cout << "weak ref null flag: "
			  << (weakptr.getptr().null() ? "null":"not null")
			  << std::endl;
	}

	std::cout << "weak ref null flag: "
		  << (weakptr.getptr().null() ? "null":"not null")
		  << std::endl;


	std::cout << "Testing weak reference" << std::endl << std::flush;

	auto wti=LIBCXX_NAMESPACE::ptr<weaktestinfo>::create();

	weaktestinfo_ptr= &*wti;

	{
		wti->strongref=testref::create();

		std::cout << "Begin thread weak test" << std::endl
			  << std::flush;

		LIBCXX_NAMESPACE::runthread<void> threadptr=({

				std::unique_lock<std::mutex>
					lock_test_mutex(weaktestinfo_ptr
							->mutex);

				auto thr=LIBCXX_NAMESPACE::run(wti);

				wti->cond.wait(lock_test_mutex);

				thr;
			});

		std::cout << "Thread started, dropping the strong reference" << std::endl << std::flush;
		wti->strongref=testref();

		threadptr->get();

		std::cout << "Thread terminated" << std::endl;
	}
	weaktestinfo_ptr=NULL;

	return 0;
}

static void objdestroy_hook()
{
	if (weaktest_go && weaktestinfo_ptr && !weaktestinfo_ptr->done)
	{
		weaktestinfo_ptr->done=true;
		std::unique_lock<std::mutex>
			lock_test_mutex(weaktestinfo_ptr->mutex);

		std::cout << "Destroying object, signaling thread"
			  << std::endl
			  << std::flush;

		weaktestinfo_ptr->cond.notify_one();

		weaktestinfo_ptr->cond.wait(lock_test_mutex);

		std::cout << "Continuing in objdestroy_hook" << std::endl;
	}
}

static void weakptrget_hook()
{
	if (weaktestinfo_ptr)
	{
		std::cout << "Hook 2!" << std::endl;

		weaktestinfo_ptr->cond.notify_one();
	}
}


#define SELFTEST_HOOK() if (weakinfop) objdestroy_hook()

#include "obj.C"

#undef SELFTEST_HOOK

#define SELFTEST_HOOK() weakptrget_hook()

#include "weakinfo.C"
