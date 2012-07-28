/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ref.H"
#include "x/obj.H"
#include "x/weakptr.H"
#include "x/exception.H"
#include "x/property_value.H"
#include "x/dequemsgdispatcher.H"
#include "x/threads/run.H"
#include "x/threads/timer.H"
#include "x/threads/timertask.H"
#include "x/threads/timertaskentryfwd.H"
#include "x/destroycallbackflag.H"
#include "x/sigset.H"
#include "x/timespec.H"
#include "x/sysexception.H"
#include "x/logger.H"

#include <unistd.h>

#include <map>
#include <deque>
#include <chrono>

namespace LIBCXX_NAMESPACE {

#include "timerobj_internal.H"
#include "timertaskentry_internal.H"
};

LOG_CLASS_INIT(LIBCXX_NAMESPACE::timerObj::implObj);

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif



timespec timespec::getclock(clockid_t clock_id)
{
	timespec ts;

	if (clock_gettime(clock_id, &ts) < 0)
		throw SYSEXCEPTION("clock_gettime");
	return ts;
}


#include "timerobj.msgs.def.H"

timerObj::timerObj() : timername("unnamed timer")
{
}

timerObj::~timerObj() noexcept
{
	cancel();
}

void timerObj::cancel()
{

	auto ret=({
			meta_container_t::lock lock(meta_container);

			{
				ptr<implObj> thr=lock->thread.getptr();

				if (!thr.null())
					thr->stop();
			}

			auto cpy=lock->thread_ret;

			lock->thread_ret=decltype(lock->thread_ret)();

			cpy;
		});

	if (!ret.null())
		ret->get(); // Wait for the thread to wrap up.
}

void timerObj::setTimerName(const std::string &nameArg) noexcept
{
	std::lock_guard<std::mutex> lock(objmutex);

	timername=nameArg;
}

static inline destroyCallbackFlagptr
launch(const timerObj::implObj::taskinfo &newtask,
       const ref<timerObj::implObj::installedObj> &installedflag,
       timerObj::meta_container_t::lock &lock,
       const std::string &timername)
{
	bool start_needed=false;

	auto p=lock->thread.getptr();

	if (p.null())
	{
		// Timer thread is not running. Wait
		// for the previous
		// thread to be done, then create a
		// new one.

		if (!lock->thread_ret.null())
			lock->thread_ret->get();

		p=ref<timerObj::implObj>::create(timername);
		start_needed=true;
	}
	else
	{
		if (p->samethread())
		{
			// Timer task scheduling more tasks for the same timer.

			installedflag->processed=true;
			p->newtask(newtask, installedflag, x::ptr<x::obj>());
			return destroyCallbackFlagptr();
		}
	}
			
	// Send the new task to the timer thread.
	destroyCallbackFlag cb=destroyCallbackFlag::create();

	{
		ref<obj> mcguffin=ref<obj>::create();

		mcguffin->addOnDestroy(cb);
		p->newtask(newtask, installedflag, mcguffin);
	}

	if (start_needed)
	{
		// This is a new timer thread. Well, start it.
		// Note that the first newtask message to the thread
		// is already queued up, so the thread
		// will find something to do, right up
		// front.

		lock->thread=p;

		sigset::block_all block_all_signals;

		lock->thread_ret=run(p);
	}
	return cb;
}

void timerObj::do_schedule(const timertask &task,
			   const time_point_t &run_time,
			   const property::propvalue &repeatProperty,
			   const duration_t &default_duration,
			   const const_locale &locale)
{
	auto newtask=ref<implObj::taskinfoObj>
		::create(task, run_time,
			 ref<repeatinfoObj>::create(repeatProperty,
						    default_duration,
						    locale));
	ref<implObj::installedObj>
		installedflag=ref<implObj::installedObj>::create();

	do
	{
		auto cb = ({
				meta_container_t::lock lock(meta_container);

				std::string n=({
						std::lock_guard<std::mutex>
							lock(objmutex);
						timername;
					});

				launch(newtask, installedflag, lock, n);
			});

		// Wait until the timer thread processes the request
		if (!cb.null())
			cb->wait();

		// Or the timer thread just terminated because it has nothing
		// else to do, so start it again.
	} while (!installedflag->processed);
}

// Timer thread. Process messages, execute jobs.

timerObj::implObj::implObj(const std::string &timernameArg)
	: timername(timernameArg)
{
}

timerObj::implObj::~implObj() noexcept
{
}

// Drain the message queue. Returns when the message queue is empty, and when
// either there are no jobs, or the next job's time has arrived.
//
// Returns the current time.

inline timerObj::clock_t::time_point
timerObj::implObj::drain()
{
	while (1)
	{
		clock_t::time_point now=clock_t::now();

		msgqueue_t::lock lock(msgqueue);

		if (!lock->empty())
		{
			auto msg=lock->front();

			lock->pop_front();
			msg->dispatch();
			continue;
		}

		if (jobs.empty())
			return now;

		auto p=jobs.begin();

		if (p->first <= now)
			return now;

		lock.wait_until(p->first, [&lock] { return !lock->empty(); });
	}
}

// Execute the next job. Returns false when there are no jobs to do.
// Returns true in all other situations.

bool timerObj::implObj::runjob()
{
	auto now=drain();

	// At this time, there are no messages in the queue. If there are jobs,
	// the first job's time has arrived.

	if (jobs.empty())
		return false;

	// So, do this job.
	// Compute the job's next run time, and remove it from the queue,
	// for now.

	auto p=jobs.begin();
	auto task=p->second;
	auto interval=task->repeat->getDuration();
	auto next_run=p->first + interval;

	jobs.erase(p);
	task->installed=false;

	try {
		task->task->run();
	} catch (const exception &e)
	{
		LOG_ERROR(e);
		LOG_TRACE(e->backtrace);
		return true;
	} catch (...)
	{
		LOG_FATAL("Unknown exception thrown");
	}

	// If the job has a non-zero repeat interval, put it back in the
	// queue. Use the original run time plus interval, but if that's already
	// water under the bridge, use the current time to reset it.

	if (interval != interval.zero())
	{
		if (next_run <= now)
			next_run=now + interval;
		task->jobentry=jobs.insert(std::make_pair(next_run, task));
		task->installed=true;
	}
	return true;
}

void timerObj::implObj::run()
{
	try {
		try {
			{
				std::lock_guard<std::mutex> lock(objmutex);

				tid=std::this_thread::get_id();

			}

			while (runjob())
				;
		} catch (const stopexception &e)
		{
		} catch (const exception &e)
		{
			LOG_FATAL(e);
			LOG_TRACE(e->backtrace);
		}
	} catch (...)
	{
		std::lock_guard<std::mutex> lock(objmutex);

		tid=std::thread::id();
		throw;
	}

	std::lock_guard<std::mutex> lock(objmutex);

	tid=std::thread::id();
}

void timerObj::implObj::dispatch(const newtask_msg &msg)
{
	msg.installed->processed=true;
	taskinfo newtask=msg.newtask;

	timertaskentry task=timertaskentry::create(newtask->task,
						   newtask->repeat);

	task->impl=ptr<implObj>(this);

	if (!task->task->install(task))
		return;
	task->installed=true;
	task->jobentry=jobs.insert(std::make_pair(newtask->run_time, task));
}

void timerObj::implObj::dispatch(const canceltask_msg &msg)
{
	auto taskentry=msg.taskentry.getptr();

	if (taskentry.null())
		return;

	if (!taskentry->installed)
		return; // Already (just removed)

	jobs.erase(taskentry->jobentry);
	taskentry->installed=false;
}

#if 0
{
#endif
}
