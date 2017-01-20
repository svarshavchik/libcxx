/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
static void debug_lockpool_hook();
#define LIBCXX_LOCKPOOL_DEBUG debug_lockpool_hook

#include "x/lockpool.H"
#include "x/exception.H"
#include "x/threads/run.H"

#include <iostream>
#include <thread>
#include <future>

static bool do_lockpool_debug=false;
static LIBCXX_NAMESPACE::eventfdptr lockpool_debug_event1,
	lockpool_debug_event2;

typedef LIBCXX_NAMESPACE::lockpool<std::string> lockpool_t;

typedef lockpool_t::base::lockentryptr lockentryptr;

lockentryptr lockpool_debug_entry;

static void debug_lockpool_hook()
{
	if (do_lockpool_debug)
	{
		lockpool_debug_event2->event(1);
		lockpool_debug_event1->event();
	}
}

static void testlockpool1()
{
	lockpool_t lp(lockpool_t::create());

	lockentryptr alock1=lp->addLockSet("A", LIBCXX_NAMESPACE::eventfd::create());

	std::cout << "alock1: " << alock1->locked()
		  << ", " << std::flush
		  << alock1->getNotifyEvent()->event() << std::endl;

	lockentryptr alock2=lp->addLockSet("A", LIBCXX_NAMESPACE::eventfd::create());

	std::cout << "alock2: " << alock2->locked() << std::endl;

	lockentryptr alock3=lp->addLockSet("A", LIBCXX_NAMESPACE::eventfd::create());

	std::cout << "alock3: " << alock3->locked() << std::endl;

	alock3=lockentryptr();

	lockentryptr block1=lp->addLockSet("B", LIBCXX_NAMESPACE::eventfd::create());

	std::cout << "block1: " << block1->locked() << std::endl;

	std::cout << "alock2: " << alock2->locked() << std::endl;

	alock1=lockentryptr();
	std::cout << "alock2: " << alock2->locked()
		  << ", " << std::flush
		  << alock2->getNotifyEvent()->event() << std::endl;

}

class lockpooltestproc : virtual public LIBCXX_NAMESPACE::obj {

public:
	void run(const lockpool_t &lp);
};

void lockpooltestproc::run(const lockpool_t &lp)
{
	lockpool_debug_event2->event();

	lockpool_debug_entry=lp->addLockSet("C", LIBCXX_NAMESPACE::eventfd::create());

	std::cout << "clock: " << lockpool_debug_entry->locked()
		  << ", " << std::flush
		  << lockpool_debug_entry->getNotifyEvent()->event()
		  << std::endl;

	lockpool_debug_event1->event(1);
}

static void testlockpool2()
{
	lockpool_t lp(lockpool_t::create());

	lockpool_debug_event1=LIBCXX_NAMESPACE::eventfd::create();
	lockpool_debug_event2=LIBCXX_NAMESPACE::eventfd::create();

	lockentryptr alock1=lp->addLockSet("A", LIBCXX_NAMESPACE::eventfd::create());

	lockentryptr alock2=lp->addLockSet("A", LIBCXX_NAMESPACE::eventfd::create());

	auto thread1Obj=LIBCXX_NAMESPACE::ref<lockpooltestproc>::create();

	auto thread1=LIBCXX_NAMESPACE::run(thread1Obj, lp);

	do_lockpool_debug=true;
	alock2=lockentryptr();
	do_lockpool_debug=false;

	thread1->get();
	lockpool_debug_entry=lockentryptr();
	lockpool_debug_event1=LIBCXX_NAMESPACE::eventfdptr();
	lockpool_debug_event2=LIBCXX_NAMESPACE::eventfdptr();
}

template<bool starve>
void testlockpoolset()
{
	typedef typename LIBCXX_NAMESPACE::multilockpool<std::string, starve>
		::lockpool_t stringlockpool;

	stringlockpool slp(stringlockpool::create());

	std::set<std::string> container1, container2, container3;

	container1.insert("A");
	container2.insert("A");
	container2.insert("B");
	container3.insert("B");

	typename stringlockpool::base::lockentryptr lock1=
		slp->addLockSet(container1, LIBCXX_NAMESPACE::eventfd::create());

	std::cout << "multilock1: " << lock1->locked() << std::endl;

	typename stringlockpool::base::lockentryptr lock2=
		slp->addLockSet(container2, LIBCXX_NAMESPACE::eventfd::create());

	std::cout << "multilock2: " << lock2->locked() << std::endl;

	typename stringlockpool::base::lockentryptr lock3=
		slp->addLockSet(container3, LIBCXX_NAMESPACE::eventfd::create());

	std::cout << "multilock3: " << lock3->locked() << std::endl;

	lock1=typename stringlockpool::base::lockentryptr();

	std::cout << "multilock2: " << lock2->locked() << std::endl;

	if (!lock2->locked())
	{
		std::cout << "multilock3: " << lock3->locked() << std::endl;
		lock3=typename stringlockpool::base::lockentryptr();
		std::cout << "multilock2: " << lock2->locked() << std::endl;
	}
	else
	{
		std::cout << "multilock3: " << lock3->locked() << std::endl;
		lock2=typename stringlockpool::base::lockentryptr();
		std::cout << "multilock3: " << lock3->locked() << std::endl;
	}
}

template<bool starve>
void testrwpoolset()
{
	typedef typename LIBCXX_NAMESPACE::sharedpool<starve> rwpool_t;

	rwpool_t rwpool(rwpool_t::create());

	typename rwpool_t::base::lockentryptr lock1=
		rwpool->addLockSet(rwpool_t::base::sharedlock, LIBCXX_NAMESPACE::eventfd::create());

	std::cout << "sharedlock: sharedlock1: " << lock1->locked()
		  << std::endl;

	typename rwpool_t::base::lockentryptr lock2=
		rwpool->addLockSet(rwpool_t::base::uniquelock, LIBCXX_NAMESPACE::eventfd::create());

	std::cout << "sharedlock: uniquelock2: " << lock2->locked()
		  << std::endl;

	typename rwpool_t::base::lockentryptr lock3=
		rwpool->addLockSet(rwpool_t::base::sharedlock, LIBCXX_NAMESPACE::eventfd::create());

	std::cout << "sharedlock: sharedlock3: " << lock3->locked()
		  << std::endl;

	lock1=typename rwpool_t::base::lockentryptr();

	std::cout << "sharedlock: uniquelock2: " << lock2->locked()
		  << std::endl;
	std::cout << "sharedlock: sharedlock3: " << lock3->locked()
		  << std::endl;

	if (!lock3->locked())
	{
		lock2=typename rwpool_t::base::lockentryptr();
		std::cout << "sharedlock: sharedlock3: " << lock3->locked()
			  << std::endl;
	}
	else
	{
		lock3=typename rwpool_t::base::lockentryptr();
		std::cout << "sharedlock: uniquelock2: " << lock2->locked()
			  << std::endl;
	}
}

void testmutexpool()
{
	typedef LIBCXX_NAMESPACE::mutexpool<> pool_t;

	typedef pool_t::base::lockentryptr lockentryptr_t;

	pool_t p=pool_t::create();

	lockentryptr_t false1=p->addLockSet(false,
					 LIBCXX_NAMESPACE::eventfd::create()),
		false2=p->addLockSet(false,
				     LIBCXX_NAMESPACE::eventfd::create()),
		true1=p->addLockSet(true,
				    LIBCXX_NAMESPACE::eventfd::create()),
		true2=p->addLockSet(true,
				    LIBCXX_NAMESPACE::eventfd::create());

	if (!false1->locked() || !false2->locked())
		throw EXCEPTION("textmutexpool failed (1)");


	if (true1->locked() || true2->locked())
		throw EXCEPTION("textmutexpool failed (2)");

	false1=false2=lockentryptr_t();

	if (!true1->locked() || !true2->locked())
		throw EXCEPTION("textmutexpool failed (3)");

}

void testlockpoolstress()
{
	std::list<std::future<void> > futures;

	typedef LIBCXX_NAMESPACE::sharedpool<> pool_t;

	pool_t pool=pool_t::create();

	for (size_t i=0; i<2; i++)
	{
		futures.push_back
			(std::async
			 (std::launch::async, [i, &pool]
			  {
				  for (size_t j=0; j<50; ++j)
				  {
					  auto w=pool->addLockSet(pool_t::base::uniquelock,
									LIBCXX_NAMESPACE::eventfd::create());
					  while (!w->locked())
						  w->getNotifyEvent()->event();
				  }
			  }));
	}

	for (size_t i=0; i<10; i++)
	{
		futures.push_back
			(std::async
			 (std::launch::async, [i, &pool]
			  {
				  for (size_t j=0; j<50; ++j)
				  {
					  auto w=pool->addLockSet(pool_t::base::sharedlock,
								  LIBCXX_NAMESPACE::eventfd::create());
					  while (!w->locked())
						  w->getNotifyEvent()->event();
				  }
			  }));
	}

	while (!futures.empty())
	{
		futures.front().get();
		futures.pop_front();
	}
}


int main(int argc, char **argv)
{
	alarm(120);
	std::cout << "Stress test" << std::endl;
	testlockpoolstress();
	std::cout << "Ok" << std::endl;
	alarm(10);

	try {
		testlockpool1();
		testlockpool2();
		testlockpoolset<false>();
		std::cout << "---" << std::endl;
		testlockpoolset<true>();
		std::cout << "---" << std::endl;

		testrwpoolset<false>();
		std::cout << "---" << std::endl;
		testrwpoolset<true>();

		{
			typedef LIBCXX_NAMESPACE::sharedpool<> rwpool_t;

			rwpool_t dummy(rwpool_t::create());
		}

		testmutexpool();

		testlockpoolstress();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
	return 0;
}
