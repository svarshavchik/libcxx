/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/threads/runthreadbaseobj.H"
#include "x/threads/runthreadobj.H"
#include "x/threads/timer.H"
#include "x/exception.H"
#include "x/singleton.H"
#include <list>
#include <sys/types.h>
#include <unistd.h>
#include <libxml/xpath.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class run_async::statics {

public:

	static std::list<ref<runthreadbaseObj> > cleanup_queue LIBCXX_HIDDEN;
};

// ----------------------------------------------------------------------------
//
// The cleanup thread.
//
// A cleanup thread gets started for the purpose of joining threads that were
// started to invoke some object's run() method. The cleanup thread gets started
// the first time a thread get created to invoke an object's run() method, by
// runthreadbaseObj's constructor.
//
// A destructor of a static object takes care of terminating the cleanup
// thread when the process gets terminated normally.

// This mutex protects all of the following static globals

std::mutex cleanup_thread_mutex LIBCXX_HIDDEN;

// The condition variable that signals the cleanup thread

std::condition_variable cleanup_thread_cond LIBCXX_HIDDEN;

bool started LIBCXX_HIDDEN=false; // The cleanup thread has been started
bool stopped LIBCXX_HIDDEN=false; // The static destructor has terminated the cleanup thread
pid_t origpid LIBCXX_HIDDEN; // Original pid of the started cleanup thread.
size_t thread_counter LIBCXX_HIDDEN=0; // How many threads are outstanding

// Terminated threads awaiting a join.

std::list<ref<runthreadbaseObj> > run_async::statics::cleanup_queue LIBCXX_HIDDEN;

std::thread *cleanup_thread LIBCXX_HIDDEN; // The actual cleanup thread

//! A static instance of this class stops the cleanup thread on app shutdown.

class LIBCXX_HIDDEN cleanup_thread_shutdown;

class cleanup_thread_shutdown {

public:
	cleanup_thread_shutdown()
	{
		xmlInitGlobals();
	}

	~cleanup_thread_shutdown()
	{
		if (getpid() == origpid)
			// Otherwise we must've forked. cleanup_thread's
			// destructor will barf, as of gcc 4.6.
			// Leak memory, can't be helped.
			do_cleanup_thread();

		// Make sure all threads stop before cleaning up libxml2
		xmlCleanupGlobals();
	}

	inline void do_cleanup_thread()
	{
		{
			std::unique_lock<std::mutex> lock(cleanup_thread_mutex);

			if (!started)
				return; // We never started.

			if (stopped)
				return;

			auto wait_for_all_threads_to_stop=[]
				{
					return thread_counter == 0;
				};

			// Give a 10 second grace period for all threads to
			// hara-kiri themselves. Then, if there are still
			// threads running, complain, and wait 50 more seconds.

			cleanup_thread_cond
				.wait_for(lock,
					  std::chrono::seconds(10),
					  wait_for_all_threads_to_stop);

			if (thread_counter)
			{
				std::cerr << "Warning: waiting for all threads to stop"
					  << std::endl;

				if (!cleanup_thread_cond
				    .wait_for(lock, std::chrono::seconds(50),
					      wait_for_all_threads_to_stop))
				{
					std::cerr << "Fatal: threads remain at exit"
						  << std::endl;
					abort();
				}
			}
			stopped=true;
			cleanup_thread_cond.notify_all();
		}

		// Wait until the cleanup thread is done.

		if (cleanup_thread->joinable())
			cleanup_thread->join();
		delete cleanup_thread;
	}
};

#include "localscope.H"

// This ensures that mainscope_destructor runs after all threads have
// stopped, by the kill_cleanup_thread

mainscope_destructor mainscope_destructor::instance;

cleanup_thread_shutdown kill_cleanup_thread LIBCXX_HIDDEN;

void cleanup_thread_func() LIBCXX_HIDDEN;

// Join threads, as they terminate.

void cleanup_thread_func()
{
	while (1)
	{
		auto thr=({
				std::unique_lock<std::mutex>
					lock(cleanup_thread_mutex);

				if (run_async::statics::cleanup_queue.empty())
				{
					if (stopped)
						break;

					cleanup_thread_cond.wait(lock);
					continue;
				}

				auto next_thr=run_async::statics::cleanup_queue.front();

				run_async::statics::cleanup_queue.pop_front();

				next_thr;
			});

		// "The library may reuse the value of a thread::id
		// of a terminated thread that can no longer be joined."
		//
		// So, until we join() this thread, below, thr_id cannot be
		// recycled. As such it is safe to rely on thr_id being unique.

		{
			std::lock_guard<std::mutex> lock(thr->objmutex);
			thr->thr_id=std::thread::id();
		}

		thr->thr.join();
		thr->joined();
	}
}

runthreadbaseObj::runthreadbaseObj()
{
	std::lock_guard<std::mutex> lock(cleanup_thread_mutex);

	if (!started)
	{
		cleanup_thread=new std::thread(cleanup_thread_func);
		origpid=getpid();
		started=true;
	}

	if (getpid() != origpid)
	{
		std::cerr << "Cannot start a thread after fork()"
			  << std::endl;
		abort();
	}
}

runthreadbaseObj::~runthreadbaseObj()
{
}

runthreadbaseObj::threadCleanupObj
::threadCleanupObj(const ref<runthreadbaseObj> &thrArg) : thr(thrArg),
							  accounted_for(false)
{
	std::lock_guard<std::mutex> lock(cleanup_thread_mutex);

	if (stopped)
		throw EXCEPTION("An attempt to start another thread was initiated when the process is stopping");
	++thread_counter;
	accounted_for=true;
}

runthreadbaseObj::threadCleanupObj::~threadCleanupObj()
{
	if (accounted_for)
	{
		std::unique_lock<std::mutex> lock(cleanup_thread_mutex);

		--thread_counter;
		cleanup_thread_cond.notify_all();
	}
}

void runthreadbaseObj::threadCleanupObj::destroyed()
{
	if (!thr->thr.joinable())
		return; // Must've been a problem starting the thread.

	std::lock_guard<std::mutex> lock(cleanup_thread_mutex);

	if (stopped)
	{
		std::cerr << "Threads remain after cleanup thread terminated"
			  << std::endl;
		abort();
	}
	run_async::statics::cleanup_queue.push_back(thr);
	cleanup_thread_cond.notify_all();
}

std::thread::id runthreadbaseObj::get_id() const
{
	std::lock_guard<std::mutex> lock(objmutex);
	return thr_id;
}

template class run_asyncthreadObj<void>;

template<>
void run_asyncthreadObj<void>::get() const
{
	typename meta_container_t::lock lock(meta);

	lock.wait([&lock] { return lock->terminated; });

	lock->shared_future.get();
}

// ------------------------------------------------------------------------

singleton<timerObj> global_timer LIBCXX_HIDDEN;

timer timerbaseObj::global()
{
	return global_timer.get();
}

#if 0
{
#endif
}
