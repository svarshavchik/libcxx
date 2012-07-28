/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/threads/workerpool.H"
#include "x/exception.H"

#include <iostream>

static std::mutex workerpool_cnt_mutex;
static std::condition_variable workerpool_cnt_condition;
static int workerpool_cnt=0;

#define LIBCXX_DEBUG_WORKEROBJ_START() do {			\
	std::lock_guard<std::mutex> lock(workerpool_cnt_mutex);	\
								\
	++workerpool_cnt;					\
	std::cout << "worker started" << std::endl;		\
	workerpool_cnt_condition.notify_all();			\
	} while(0)

#define LIBCXX_DEBUG_WORKEROBJ_STOP() do {			\
	std::lock_guard<std::mutex> lock(workerpool_cnt_mutex);	\
								\
	std::cout << "worker exited" << std::endl;		\
	--workerpool_cnt;					\
	workerpool_cnt_condition.notify_all();			\
	} while(0)


#include "workerpoolobj.C"

class testjob : virtual public LIBCXX_NAMESPACE::obj {

public:
	std::mutex m;
	std::condition_variable c;
	bool flag;
	int waiting;
	int total;

	testjob() : flag(true), waiting(0), total(0)
	{
	}

	~testjob() noexcept {
	}

	void run()
	{
	}

	void run(bool flagwanted)
	{
		std::unique_lock<std::mutex> lock(m);

		++waiting;
		c.notify_all();
		c.wait(lock, [flagwanted, this]
		       {
			       return flag == flagwanted;
		       });
		--waiting;
		++total;
		c.notify_all();
	}
};

void test1()
{
	LIBCXX_NAMESPACE::workerpoolptr<> pool=
		LIBCXX_NAMESPACE::workerpoolptr<>::
		create(4, 6, "worker", L"workerpool");

	auto job=LIBCXX_NAMESPACE::ref<testjob>::create();

	pool->run(job);

	{
		std::unique_lock<std::mutex> l(workerpool_cnt_mutex);

		workerpool_cnt_condition.wait(l, [=]
					      {
						      return workerpool_cnt
							      == 4;
					      });
	}
	std::cout << "Reached 4 workers" << std::endl;

	LIBCXX_NAMESPACE::property::load_property(L"workerpool::min", L"2",
						true, true);

	pool->run(job);

	{
		std::unique_lock<std::mutex> l(workerpool_cnt_mutex);

		workerpool_cnt_condition.wait(l, [=]
					      {
						      return workerpool_cnt
							      == 2;
					      });
	}
	std::cout << "Reached 2 workers" << std::endl;

	pool=LIBCXX_NAMESPACE::workerpoolptr<>();
	std::cout << "Destroyed" << std::endl;
	{
		std::unique_lock<std::mutex> l(workerpool_cnt_mutex);

		workerpool_cnt_condition.wait(l, [=]
					      {
						      return workerpool_cnt
							      == 0;
					      });
	}

	pool=LIBCXX_NAMESPACE::workerpoolptr<>::create(4, 6, "worker",
						     L"workerpool");

	for (size_t i=0; i<3; ++i)
		pool->run(job, false);

	{
		std::unique_lock<std::mutex> l(job->m);

		job->c.wait(l, [&] { return job->waiting == 3; } );
	}

	size_t started=({
			std::unique_lock<std::mutex> l(workerpool_cnt_mutex);

			workerpool_cnt;
		});

	if (started != 3)
	{
		std::unique_lock<std::mutex> l(job->m);

		job->flag=false;
		job->c.notify_all();
		throw EXCEPTION("Did not start 3 jobs, as expected");
	}

	std::cout << "Starting 4th job" << std::endl;

	pool->run(job, false);

	{
		std::unique_lock<std::mutex> l(job->m);

		job->c.wait(l, [&] { return job->waiting == 4; } );

		std::cout << "Starting 6 more jobs" << std::endl;

		for (size_t i=0; i<6; ++i)
			pool->run(job, false);

		job->c.wait(l, [&] { return job->waiting == 6; } );

		job->flag=false;
		job->c.notify_all();

		job->c.wait(l, [&] { return job->total == 10; } );
		std::cout << "Seen all 10 jobs" << std::endl;
	}

	while ( ({
				std::unique_lock<std::mutex>
					l(workerpool_cnt_mutex);
				workerpool_cnt;
			}) != 2)
	{
		std::unique_lock<std::mutex> l(job->m);

		job->flag=true;

		pool->run(job, false);

		job->c.wait(l, [&] { return job->waiting == 1; });
		job->flag=false;
		job->c.notify_all();

		job->c.wait(l, [&] { return job->waiting == 0; });
	}
}

int main()
{
	alarm(30);
	test1();

}
