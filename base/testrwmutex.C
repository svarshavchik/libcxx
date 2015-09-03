/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/rwmutex.H"
#include "x/rwmutexdebug.H"
#include "x/exception.H"
#include "x/threads/run.H"
#include <thread>
#include <mutex>
#include <thread>
#include <future>
#include <iostream>
#include <stdlib.h>

void testrwmutex()
{
	LIBCXX_NAMESPACE::rwmutex rw;

	{
		std::unique_lock<LIBCXX_NAMESPACE::rwmutex::rmutex> r1(rw.r);
		std::unique_lock<LIBCXX_NAMESPACE::rwmutex::rmutex> r2(rw.r);

		if (!r1.owns_lock() || !r2.owns_lock())
			throw EXCEPTION("How come read locks are not owners?");

		std::unique_lock<LIBCXX_NAMESPACE::rwmutex::wmutex>
			w1(rw.w, std::try_to_lock_t());

		if (w1.owns_lock())
			throw EXCEPTION("How did the write lock succeed?");
	}

	{
		std::unique_lock<LIBCXX_NAMESPACE::rwmutex::wmutex> w1(rw.w);
		std::unique_lock<LIBCXX_NAMESPACE::rwmutex::rmutex>
			r1(rw.r, std::try_to_lock_t());

		if (r1.owns_lock())
			throw EXCEPTION("How did the read lock succeed, with the write lock in existence?");

		std::unique_lock<LIBCXX_NAMESPACE::rwmutex::wmutex>
			w2(rw.w, std::try_to_lock_t());

		if (w2.owns_lock())
			throw EXCEPTION("How did the second write lock succeed?");
	}

	for (int i=0; i<2; ++i)
	{
		std::unique_lock<LIBCXX_NAMESPACE::rwmutex::wmutex> w1(rw.w);

		std::thread thr( [&rw] {
				std::cout << "Thread waiting for lock"
					  << std::endl;
				std::unique_lock<LIBCXX_NAMESPACE::rwmutex
						 ::rmutex> r1(rw.r);
				std::cout << "Thread acquired for lock"
					  << std::endl;
			});

		sleep(1);

		w1.unlock();
		thr.join();
	}

	{
		volatile bool flag=false;
		bool flag_save;

		std::thread thr;

		{
			std::unique_lock<LIBCXX_NAMESPACE::rwmutex::rmutex>
				r1(rw.r);
			{
				std::unique_lock<LIBCXX_NAMESPACE::rwmutex
						 ::rmutex>
					r2(rw.r);

				thr=std::thread([&rw, &flag] {

						std::cout << "Thread waiting for lock"
							  << std::endl;
						std::unique_lock<LIBCXX_NAMESPACE::rwmutex
								 ::wmutex> r1(rw.w);
						std::cout << "Thread acquired for lock"
							  << std::endl;
						flag=true;
					});
				sleep(1);
			}
			sleep(1);
			flag_save=flag;
		}
		thr.join();

		if (flag_save)
			throw EXCEPTION("Write lock succeeded even though one read lock was still left");
		if (!flag)
			throw EXCEPTION("No evidence that the write lock succeeded?");
	}

	{
		int who=0;
		std::mutex who_mutex;
		std::condition_variable who_cond;

		std::unique_lock<LIBCXX_NAMESPACE::rwmutex::rmutex> r1(rw.r);

		std::thread thr1([&rw, &who, &who_mutex, &who_cond] {
				std::cout << "Write thread waiting for lock"
					  << std::endl;
				std::unique_lock<LIBCXX_NAMESPACE::rwmutex
						 ::wmutex> w(rw.w);
				std::cout << "Write thread acquired lock"
					  << std::endl;

				std::unique_lock<std::mutex> wl(who_mutex);

				who=1;
				who_cond.wait(wl, [&who] { return who != 1; } );
			});

		sleep(1);

		std::thread thr2([&rw, &who, &who_mutex, &who_cond] {

				std::unique_lock<LIBCXX_NAMESPACE::rwmutex
						 ::rmutex> r2(rw.r);
				std::unique_lock<std::mutex> wl(who_mutex);

				who=3;
				who_cond.notify_all();
			});
		sleep(1);

		{
			std::unique_lock<std::mutex> wl(who_mutex);

			if (who)
			{
				std::cout << "Locking order error (not 0): " << who
					  << std::endl;
				abort();
			}

			r1=std::unique_lock<LIBCXX_NAMESPACE::rwmutex::rmutex>();
		}

		sleep(1);

		{
			std::unique_lock<std::mutex> wl(who_mutex);

			if (who != 1)
			{
				std::cout << "Locking order error (not 1): " << who
					  << std::endl;
				abort();
			}
			who=2;
			who_cond.notify_all();

			who_cond.wait(wl, [&who] { return who != 3; } );
		}
		thr1.join();
		thr2.join();
	}

	std::cout << "testing timed wait on a write lock" << std::endl;

	{
		std::unique_lock<LIBCXX_NAMESPACE::rwmutex::rmutex> r1(rw.r);

		{
			std::unique_lock<LIBCXX_NAMESPACE::rwmutex::wmutex>
				l(rw.w, std::chrono::seconds(1));

			if (l.owns_lock())
				throw EXCEPTION("try_lock_for(w) fail");
		}

		{
			std::unique_lock<LIBCXX_NAMESPACE::rwmutex::wmutex>
				l(rw.w, std::chrono::system_clock::now()
				  + std::chrono::seconds(1));

			if (l.owns_lock())
				throw EXCEPTION("try_lock_until(w) fail");
		}
	}

	std::cout << "testing timed wait on a read lock" << std::endl;

	{
		std::unique_lock<LIBCXX_NAMESPACE::rwmutex::wmutex> r1(rw.w);

		{
			std::unique_lock<LIBCXX_NAMESPACE::rwmutex::rmutex>
				l(rw.r, std::chrono::seconds(1));

			if (l.owns_lock())
				throw EXCEPTION("try_lock_for(r) fail");
		}

		{
			std::unique_lock<LIBCXX_NAMESPACE::rwmutex::rmutex>
				l(rw.r, std::chrono::system_clock::now()
				  + std::chrono::seconds(1));

			if (l.owns_lock())
				throw EXCEPTION("try_lock_until(r) fail");
		}
	}
}

void testrwmutex2()
{
	LIBCXX_NAMESPACE::rwmutex rw;

	std::list<std::future<void> > futures;

	for (size_t i=0; i<100;++i)
	{
		futures.push_back
			(std::async
			 (std::launch::async, [i, &rw]
			  {
				  for (size_t j=0; j<10000; ++j)
				  {
					  std::unique_lock<LIBCXX_NAMESPACE
							   ::rwmutex::rmutex>
						  lock(rw.r);
				  }
				  std::cout << "Read "
					    << i
					    << std::endl;
			  }));
	}

	for (size_t i=0; i<2;++i)
	{
		futures.push_back
			(std::async
			 (std::launch::async, [i, &rw]
			  {
				  for (size_t j=0; j<10000; ++j)
				  {
					  std::unique_lock<LIBCXX_NAMESPACE
							   ::rwmutex::wmutex>
						  lock(rw.w);
				  }
				  std::cout << "Write "
					    << i
					    << std::endl;
			  }));
	}

	while (!futures.empty())
	{
		futures.front().get();
		futures.pop_front();
	}
}

class testrwmutexdebugObj : virtual public LIBCXX_NAMESPACE::obj {

public:

	LIBCXX_NAMESPACE::rwmutexdebug debug;

	std::mutex m;
	std::condition_variable c;
	bool locked;

	testrwmutexdebugObj() : locked(false) {}
	~testrwmutexdebugObj() noexcept {}

	void run()
	{
		std::unique_lock<std::mutex> l(m);

		std::lock_guard<LIBCXX_NAMESPACE::rwmutexdebug::rmutex>
			ll(debug.r);

		locked=true;
		c.notify_all();

		c.wait(l, [&] { return !locked; });
	}
};

void testrwmutexdebug()
{
	auto test=LIBCXX_NAMESPACE::ref<testrwmutexdebugObj>::create();

	auto thread=LIBCXX_NAMESPACE::run(test);

	{
		std::unique_lock<std::mutex> l(test->m);

		test->c.wait(l, [&test] { return test->locked; });

		std::lock_guard<LIBCXX_NAMESPACE::rwmutexdebug::rmutex>
			lock1(test->debug.r);

		test->locked=false;
		test->c.notify_all();

		bool caught=false;

		try {
			std::lock_guard<LIBCXX_NAMESPACE::rwmutexdebug::wmutex>
				(test->debug.w);
		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			caught=true;
			std::cerr << "Caught expected exception: " << e
				  << std::endl;
		} catch (...)
		{
			std::cerr << "HUH!" << std::endl;
		}

		if (!caught)
			throw EXCEPTION("rwmutexdebug didn't work as expected");
	}
	thread->wait();
}


int main(int argc, char **argv)
{
	alarm(60);
	testrwmutex();
	testrwmutex2();
	testrwmutexdebug();
	return 0;
}
