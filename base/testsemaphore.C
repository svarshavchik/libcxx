/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "semaphore.H"

#include <vector>
#include <iostream>
#include <mutex>
#include <condition_variable>

class testowner : public LIBCXX_NAMESPACE::semaphore::base::ownerObj {

public:

	LIBCXX_NAMESPACE::semaphore sem;

	std::vector<LIBCXX_NAMESPACE::ptr<testowner> > reqs;

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

		for (std::vector<LIBCXX_NAMESPACE::ptr<testowner> >::iterator
			     b=reqs.begin(), e=reqs.end(); b != e; ++b)
			sem->request(*b, 1);
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
		three(L"two", 2);

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
		three(L"two", 2);

	LIBCXX_NAMESPACE::semaphore sem(LIBCXX_NAMESPACE::semaphore::create(three));

	own vec[3]={own::create(sem),
		    own::create(sem),
		    own::create(sem)};

	vec[0]->reqs.push_back(vec[1]);
	vec[0]->reqs.push_back(vec[2]);

	sem->request(vec[0], 4);

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

int main()
{
	alarm(60);
	try {
		testsemaphore1();
		testsemaphore2();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
