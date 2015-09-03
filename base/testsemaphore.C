/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/semaphore.H"
#include "x/fixed_semaphore.H"

#include <vector>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <stdlib.h>

class testowner : public LIBCXX_NAMESPACE::semaphore::base::ownerObj {

public:

	LIBCXX_NAMESPACE::semaphore sem;

	std::vector<LIBCXX_NAMESPACE::ref<testowner> > reqs;

	std::mutex mutex;
	std::condition_variable cond;
	bool flag;

	testowner(const LIBCXX_NAMESPACE::semaphore &semArg)
		: sem(semArg), flag(false)
	{
	}

	~testowner() noexcept
	{
	}

	void process(const LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj> &mcguffin)

	{
		{
			std::lock_guard<std::mutex> lock(mutex);

			flag=true;
			cond.notify_all();
		}

		for (const auto &req: reqs)
			sem->request(req, 1);
	}

	void wait()
	{
		std::unique_lock<std::mutex> lock(mutex);

		while (!flag)
			cond.wait(lock);
	}
};

typedef LIBCXX_NAMESPACE::ref<testowner> own;

void testsemaphore1()
{
	static LIBCXX_NAMESPACE::property::value<size_t>
		three("two", 2);

	LIBCXX_NAMESPACE::semaphore sem(LIBCXX_NAMESPACE::semaphore::create(three));

	own vec[3]={own::create(sem),
		    own::create(sem),
		    own::create(sem)};

	vec[0]->reqs.push_back(vec[1]);
	vec[0]->reqs.push_back(vec[2]);

	sem->request(vec[0], 1);
	vec[0]->wait();
	vec[1]->wait();
	vec[2]->wait();
}

void testsemaphore2()
{
	static LIBCXX_NAMESPACE::property::value<size_t>
		three("two", 2);

	LIBCXX_NAMESPACE::semaphore sem(LIBCXX_NAMESPACE::semaphore::create(three));

	own vec[3]={own::create(sem),
		    own::create(sem),
		    own::create(sem)};

	vec[0]->reqs.push_back(vec[1]);
	vec[0]->reqs.push_back(vec[2]);

	auto owner=LIBCXX_NAMESPACE::semaphore::base::owner::create
		([vec]
		 (const LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj> &mcguffin)
		 {
			 vec[0]->process(mcguffin);
		 });

	sem->request(owner, 4);
	vec[0]->wait();
	vec[1]->wait();
	vec[2]->wait();

	vec[0]->flag=false;
	vec[1]->flag=false;
	vec[2]->flag=false;

	sem->request(vec[0], 4);
	vec[0]->wait();
	vec[1]->wait();
	vec[2]->wait();
}

void testfixedsemaphore()
{
	auto s=LIBCXX_NAMESPACE::fixed_semaphore::create(3);

	if (!s->acquire([]
			{
				return true;
			},
			[]
			{
			}, 4).null())
		throw EXCEPTION("Shouldn't acquire 4 when there are 3 available");

	typedef LIBCXX_NAMESPACE::fixed_semaphore::base::acquiredptr
		acquiredptr;

	if (!s->acquire([]
			{
				return false;
			},
			[]
			{
			}, 3).null())
		throw EXCEPTION("Acquisition should've failed");

	acquiredptr a=s->acquire([]
				 {
					 return true;
				 },
				 []
				 {
				 }, 2);

	if (a.null())
		throw EXCEPTION("a did not acquire");

	acquiredptr b=s->acquire([]
				 {
					 return true;
				 },
				 []
				 {
				 }, 2);

	if (!b.null())
		throw EXCEPTION("b acquired");

	b=s->acquire([]
		     {
			     return true;
		     },
		     []
		     {
		     }, 1);

	if (b.null())
		throw EXCEPTION("b did not acquire");

	acquiredptr c=s->acquire([]
				 {
					 return true;
				 },
				 []
				 {
				 }, 2);

	if (!c.null())
		throw EXCEPTION("c acquired");

	a=acquiredptr();

	c=s->acquire([]
		     {
			     return true;
		     },
		     []
		     {
		     }, 2);
	if (c.null())
		throw EXCEPTION("c should've acquired");

}

int main()
{
	alarm(60);
	try {
		testsemaphore1();
		testsemaphore2();
		testfixedsemaphore();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
