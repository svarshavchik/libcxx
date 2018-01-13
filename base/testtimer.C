/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/threads/timer.H"
#include "x/threads/timertask.H"
#include "x/destroy_callback.H"

class timerCount : public LIBCXX_NAMESPACE::timertaskObj {

	int countdown;


	const char *name;

public:
	timerCount(int n, const char *nameArg) : countdown(n), name(nameArg)
	{
	}

	~timerCount()
	{
		std::cout << "destructor, thread " << name << std::endl;
	}

	void run() override
	{
		std::cout << "run, thread " << name
			  << std::endl;
		if (--countdown == 0)
			cancel();
	}
};

static void testtimertask() noexcept
{
	auto timer=LIBCXX_NAMESPACE::timerptr::create();

	{
		auto cb=LIBCXX_NAMESPACE::destroy_callback::create();


		{
			auto job=LIBCXX_NAMESPACE::ref<timerCount>
				::create(4, "a");

			timer->scheduleAtFixedRate(job,
						   std::chrono::milliseconds(250));
			job->ondestroy([cb] {cb->destroyed();});
		}
		cb->wait();
	}



	{
		auto cb=LIBCXX_NAMESPACE::destroy_callback::create();


		{
			auto job=LIBCXX_NAMESPACE::ref<timerCount>
				::create(4, "b");

			timer->scheduleAtFixedRate(job,
						   std::chrono::seconds(60));
			job->ondestroy([cb]{cb->destroyed();});
			job->cancel();
		}
		cb->wait();
	}

	{
		auto cb=LIBCXX_NAMESPACE::destroy_callback::create();


		{
			auto job=LIBCXX_NAMESPACE::ref<timerCount>
				::create(4, "c");

			timer->scheduleAtFixedRate(job,
						   std::chrono::seconds(60));
			job->ondestroy([cb]{cb->destroyed();});
		}

		timer=LIBCXX_NAMESPACE::timerptr::create();
		cb->wait();
	}
}


class dummyThread : public LIBCXX_NAMESPACE::timertaskObj {

public:
	std::mutex mutex;
	std::condition_variable cond;

	dummyThread() noexcept {}
	~dummyThread() {}

	void run() override
	{
		std::cout << "In dummy thread, sleeping" << std::endl;

		{
			std::lock_guard<std::mutex> m(mutex);
			cond.notify_all();
		}

		sleep(3);
		std::cout << "In dummy thread, slept" << std::endl;
	}
};

static void testtimertask2() noexcept
{
	auto timer=LIBCXX_NAMESPACE::timer::create();

	LIBCXX_NAMESPACE::ref<dummyThread>
		dummytask(LIBCXX_NAMESPACE::ref<dummyThread>::create());

	std::cout << "testtimertask2: Starting dummy thread"
		  << std::endl;

	{
		std::unique_lock<std::mutex> l(dummytask->mutex);

		timer->scheduleAfter(dummytask, std::chrono::seconds(0));
		dummytask->cond.wait(l);
	}

	std::cout << "Cancelling" << std::endl;

	dummytask->cancel();

	std::cout << "Cancelled" << std::endl;
}

static void testtimertask3() noexcept
{
	auto timer=LIBCXX_NAMESPACE::timer::create();

	std::cout << "testtimer3task" << std::endl;

	LIBCXX_NAMESPACE::property
		::load_properties("timer.rate=1 second\n",
				  true, true,
				  LIBCXX_NAMESPACE::property::errhandler::errthrow(),
				  LIBCXX_NAMESPACE::locale::create("C"));

	LIBCXX_NAMESPACE::destroy_callback flag=
		LIBCXX_NAMESPACE::destroy_callback::create();

	{
		auto counter=LIBCXX_NAMESPACE::ptr<timerCount>::create(3, "3");

		timer->scheduleAtFixedRate(counter, "timer.rate",
					   std::chrono::seconds(600));

		counter->ondestroy([flag]{flag->destroyed();});
	}

	flag->wait();
}

class selfcancel : public LIBCXX_NAMESPACE::timertaskObj {

public:
	selfcancel() {}
	~selfcancel() {}

	void run() override
	{
		cancel();
		cancel();
	}
};

static void testtimertask4() noexcept
{
	auto timer=LIBCXX_NAMESPACE::timerptr::create();

	std::cout << "testtimer4task" << std::endl;

	auto cb1=LIBCXX_NAMESPACE::destroy_callback::create();
	auto cb2=LIBCXX_NAMESPACE::destroy_callback::create();


	{

		auto timerjob=LIBCXX_NAMESPACE::ref<timerCount>
			::create(4, "4");

		timerjob->ondestroy([cb1]{cb1->destroyed();});

		timer->scheduleAtFixedRate(timerjob,
					   std::chrono::seconds(1));

	}

	{

		auto canceljob=LIBCXX_NAMESPACE::ref<selfcancel>::create();

		canceljob->ondestroy([cb2]{cb2->destroyed();});

		timer->schedule(canceljob,
				std::chrono::system_clock::now()
				+ std::chrono::milliseconds(1500));
	}

	cb2->wait();
	std::cout << "Cancelled job 1" << std::endl;
	timer=LIBCXX_NAMESPACE::timerptr();
	cb1->wait();
	std::cout << "Cancelled job 2" << std::endl;
}

class timertask5Obj : virtual public LIBCXX_NAMESPACE::obj {

public:
	std::mutex m;
	std::condition_variable c;
	bool flag;
	int answer;

	timertask5Obj() : flag(false)
	{
	}

	void run(int a, int b)
	{
		std::lock_guard<std::mutex> l(m);

		answer=a+b;
		flag=true;
		c.notify_all();
	}
};

void testtimertask5()
{
	std::cout << "testtimertask5" << std::endl;

	auto timer=LIBCXX_NAMESPACE::timer::create();

	auto tt=LIBCXX_NAMESPACE::ref<timertask5Obj>::create();

	timer->scheduleAfter(LIBCXX_NAMESPACE::timertask::base
			     ::make_timer_task([tt]
					       {
						       tt->run(2, 2);
					       }),
			     std::chrono::seconds(1));

	std::unique_lock<std::mutex> l(tt->m);

	tt->c.wait(l, [&tt] { return tt->flag; });

	if (tt->answer != 4)
		throw EXCEPTION("testtimertask5 failed");
}

class testtimertask6Obj : virtual public LIBCXX_NAMESPACE::obj {

 public:
	std::mutex mutex;
	std::condition_variable cond;
	int n;

	testtimertask6Obj() : n(2) {}
	~testtimertask6Obj() {
	}

	void run(const LIBCXX_NAMESPACE::timer &t)
	{
		std::lock_guard<std::mutex> lock(mutex);

		std::cout << --n << std::endl;

		cond.notify_all();

		if (n)
		{
			auto this_task=LIBCXX_NAMESPACE::ref<testtimertask6Obj>
				(this);

			auto functor=[this_task, t]
				{
					this_task->run(t);
				};

			t->scheduleAfter(LIBCXX_NAMESPACE::timertask::base
					 ::make_timer_task(functor),
					 std::chrono::seconds(1));
		}
	}
};

void testtimertask6()
{
	std::cout << "testtimertask6" << std::endl;

	auto timer=LIBCXX_NAMESPACE::timer::create();

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto ptr=x::ref<testtimertask6Obj>::create();

	guard(ptr);

	auto task=LIBCXX_NAMESPACE::timertask::base
			     ::make_timer_task([ptr, timer]
					       {
						       ptr->run(timer);
					       });
	auto mcguffin=task->autocancel();

	timer->scheduleAfter(task, std::chrono::seconds(1));

	std::unique_lock<std::mutex> lock(ptr->mutex);

	ptr->cond.wait(lock, [&ptr] { return ptr->n == 0; });
}

void testtimertask7()
{
	std::cout << "testtimertask7" << std::endl;

	auto timer=LIBCXX_NAMESPACE::timer::create();

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto ptr=x::ref<testtimertask6Obj>::create();

	guard(ptr);

	auto task=LIBCXX_NAMESPACE::timertask::base
		::make_timer_task([ptr, timer]
				  {
					  ptr->run(timer);
				  });

	auto mcguffin=task->autocancel();
	timer->scheduleAfter(task,
			     std::chrono::minutes(5));
	sleep(2);
}

static LIBCXX_NAMESPACE::timer::base::duration_property_t
dummy("duration", std::chrono::seconds(2));

int main()
{
	alarm(60);
	testtimertask();
	testtimertask2();
	testtimertask3();
	testtimertask4();
	testtimertask5();
	testtimertask6();
	testtimertask7();
	return 0;
}
