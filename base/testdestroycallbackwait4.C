/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/destroycallbackflagwait4.H"
#include "x/threads/run.H"
#include "x/exception.H"
#include <unistd.h>
#include <iostream>

typedef LIBCXX_NAMESPACE::destroyCallbackFlagWait4 w4;

class test1Obj : virtual public LIBCXX_NAMESPACE::obj {

public:
	LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj> mcguffin;

	std::mutex m;
	std::condition_variable c;
	bool flag;

	test1Obj():
		mcguffin(LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>::create()),
		flag(false)
	{
	}

	~test1Obj() noexcept {
	}

	void run()
	{
		{
			std::lock_guard<std::mutex> g(m);

			flag=true;
			c.notify_all();
		}

		{
			std::cout << "Saving mcguffin" << std::endl;
			auto save = mcguffin;

			mcguffin = LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>
				::create();
		}
		std::cout << "Destroyed mcguffin" << std::endl;


		{
			std::lock_guard<std::mutex> g(m);

			flag=false;
			c.notify_all();
		}
	}
};


static void testdestroycallbackwait4()
{
	{
		auto t1=LIBCXX_NAMESPACE::ref<test1Obj>::create();

		auto w=w4::create(t1->mcguffin);

		t1->ondestroy([w] {w->destroyed();});
	}

	{
		auto t1=LIBCXX_NAMESPACE::ref<test1Obj>::create();

		auto mcguffin2=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>
			::create();

		auto w=w4::create(mcguffin2);

		t1->mcguffin->ondestroy([w] {w->destroyed();});

		auto ret=LIBCXX_NAMESPACE::run(t1);

		{
			std::unique_lock<std::mutex> l(t1->m);

			t1->c.wait(l, [&t1] { return t1->flag; });
		}

		sleep(1);

		{
			std::unique_lock<std::mutex> l(t1->m);

			if (!t1->flag)
				throw EXCEPTION("Something's borked");

			mcguffin2=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>();

			t1->c.wait(l, [&t1] { return !t1->flag; });
		}
		ret->get();
	}
}

int main(int argc, char **argv)
{
	alarm(30);
	testdestroycallbackwait4();
	return 0;
}
