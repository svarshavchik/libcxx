/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_workerpoolobj_H
#define x_workerpoolobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/ptrfwd.H>
#include <x/mpobj.H>
#include <x/weaklist.H>
#include <x/property_value.H>
#include <x/threads/run.H>
#include <x/sigset.H>
#include <x/logger.H>
#include <deque>
#include <tuple>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for worker pool objects, with stuff that does not vary based on the parameter type

//! \internal
//!
class workerpoolbase {

protected:

	//! Logging

	LOG_CLASS_SCOPE;

	//! Base class for worker threads

	//! The actual worker thread class object multiple inherits from
	//! workerpool's template parameter, and this object.

	class workerbaseObj : public runthreadname, virtual public obj {


		//! The thread's name
		std::string name;

		//! Retrieve the thread's name, for logging purposes.

		std::string getName() const override;
	public:
		//! Constructor
		workerbaseObj(const std::string &nameArg);

		//! Destructor
		~workerbaseObj();
	};

	//! Property, minimum number of threads to start

	property::value<size_t> minthreadcount_prop;

	//! Property, maximum number of threads to start
	property::value<size_t> maxthreadcount_prop;

	//! Property, thread names, for logging purposes
	property::value<std::string> threadnames_prop;

	//! The container for started, and still running worker threads.
	typedef weaklist<runthread<void>::obj_type> workers_t;


	//! Started, and still running worker threads.

	//! Their return objects, actually. The weak references go out of scope
	//! when worker threads stop. The workerpool's destructor tells any
	//! remaining threads to stop, then waiting for anything that's left
	//! here.
	workers_t workers;

	//! Constructor
	workerpoolbase(const std::string &prophier,
		       size_t default_minthreadcount,
		       size_t default_maxthreadcount,
		       const std::string &threadnameArg);

	//! Construct a property name for one of the used properties.
	static std::string
	getpropname(const std::string &prophier,
		    const std::string &name) LIBCXX_INTERNAL;

	//! A run() terminated with an exception, log it.
	static void captured_exception(const exception &e);

	//! Some unknown exception was thrown by run(), log it.
	static void captured_unknown_exception();

public:

	//! Get the maximum number of thread jobs

	size_t getMaxthreads() const
	{
		return maxthreadcount_prop.get();
	}

	//! Get the minimum number of thread jobs

	size_t getMinthreads() const
	{
		return minthreadcount_prop.get();
	}
};

//! Implement a pool of worker threads.

template<typename workerThreadType>
class workerpoolObj : public workerpoolbase, virtual public obj {

	class jobqueueObj;
	class workerObj;

	//! Base class for run() job objects.

	class jobObj : virtual public obj {

	public:

		//! Keep the run() caller's signal mask for the run() call.

		sigset sigmask;

		jobObj()
		{
			sigmask=sigset::current();
		}
		~jobObj()
		{
		}

		//! Implement the job, invoking the forwarded run() method.

		virtual void job(workerThreadType *)=0;
	};

	//! The worker thread

	//! The worker thread multiply inherits from the base class,
	//! implements a top level() run() method that pulls jobs off the
	//! shared queue, and invokes them using the worker therad template
	//! class.

	class workerObj : public workerbaseObj, public workerThreadType {

	public:
		//! Constructor, forwarded the thread name.

		workerObj(const std::string &nameArg)
			: workerbaseObj(nameArg)
		{
		}

		//! Destructor
		~workerObj()
		{
		}

		void run(const ref<jobqueueObj> &queue)
			noexcept;
	};

	//! Metadata for the job queue, run() calls for worker thread to invoke.

	class jobqueue_meta {

	public:
		//! The pending run() calls.

		std::deque<ref<jobObj> > pending_jobs;

		//! How many worker threads have been started.
		size_t threadcount_started;

		//! How many run calls() have been added to pending_jobs, and have not yet been finished

		size_t unprocessed_jobs;

		//! Termination queue

		//! When it's necesary to terminate one of the worker threads,
		//! a dummy object goes in here. The first available thread
		//! that takes it off, stops itself.
		//!
		//! threadcount_started gets decremented immediately after
		//! adding a dummy object here, before the thread actually
		//! stops.

		std::deque<ref<obj> > terminate_mcguffins;

		//! Constructor
		jobqueue_meta() : threadcount_started(0),
				  unprocessed_jobs(0)
		{
		}

		//! Stop one of the threads.
		void stop1()
		{
			terminate_mcguffins.push_back(ref<obj>::create());
			--threadcount_started;
		}
	};

	//! A mutex-protected container for the metadata.

	typedef mpcobj<jobqueue_meta> jobqueue_t;

	//! The reference-counted metadata.

	//! The workerpool object, and each thread, reference this object.

	class jobqueueObj : virtual public obj {

	public:
		//! The job queue.

		jobqueue_t jobqueue;

		//! Constructor
		jobqueueObj()
		{
		}

		//! Destructor
		~jobqueueObj()
		{
		}
	};

	//! The worker pool's job queue.

	mutable ref<jobqueueObj> jobqueue;

	//! A queued-up run() call.

	//! The run()s parameter get saved in a tuple. A worker thread invokes
	//! job(), which unpacks the tuple, and invokse the worker thread
	//! object's run() method.

	template<typename ...Args>
	class runjobObj : public jobObj {

	public:
		//! Saved arguments to run().

		std::tuple<Args...> args;

		//! Constructor places the arguments into a tuple.

		template<typename ...constructorArgs>
		runjobObj(constructorArgs && ...argsArg)
			: args(std::forward<constructorArgs>(argsArg)...)
		{
		}

		//! Invoke the worker therad object's run() method.
		void job(workerThreadType *worker) override
		{
			run_invoke<void>::invoke(worker, args);
		}
	};

	void addJob(const ref<jobObj> &job);

public:
	//! Create a pool of worker threads()

	workerpoolObj(//! Minimum number of threads
		      size_t default_minthreadcount,

		      //! Maximum number of threads
		      size_t default_maxthreadcount,

		      //! The name of each worker thread, for logging purposes.

		      const std::string &threadnameArg="worker",

		      //! Property hierarchy

		      //! If set, the "prophier::min" property sets the minimum
		      //! number of worker threads, "prophier::max" sets the
		      //! maximum, and "prophier::name" gives their name, for
		      //! logging purposes; the above become the default values
		      //! if the corresponding property is not set.
		      const std::string &prophier="")
		: workerpoolbase(prophier, default_minthreadcount,
				 default_maxthreadcount, threadnameArg),
		  jobqueue(ref<jobqueueObj>::create())
	{
	}

	//! Destructor
	~workerpoolObj()
	{
		{
			// Stop all workers

			typename jobqueue_t::lock lock(jobqueue->jobqueue);

			while (lock->threadcount_started > 0)
			{
				lock->stop1();
				lock.notify_all();
			}
		}

		// Wait for them to stop

		for (auto worker:*workers)
		{
			auto ptr=worker.getptr();

			if (!ptr.null())
				ptr->wait();
		}
	}

	//! Invoke run() in an available worker thread.

	//! The parameters are forwarded to an available worker thread's run()
	//! method. If all worker threads are busy, the parameters get saved
	//! and the run() gets invoked when a thread becomes available.
	//!
	//! All arguments must be copy-constructible.
	//!
	template<typename ...Args>
	void run(Args && ...args)
	{
		addJob(ref<runjobObj<typename std::decay<Args>::type...> >
		       ::create(std::forward<Args>(args)...));
	}

	//! Return the number of run() requests that have not been completed.
	size_t getPendingCount() const
	{
		typename jobqueue_t::lock lock(jobqueue->jobqueue);

		return lock->unprocessed_jobs;
	}

};

//! A worker thread

//! Pick jobs, and invoke them. Listen for termination mcguffins.
//!
//! \internal

template<typename workerThreadType>
void workerpoolObj<workerThreadType>
::workerObj::run(const ref<jobqueueObj> &queue) noexcept
{
	while (1)
	{
		// Get the next job.
		ref<jobObj> job=({
				typename jobqueue_t::lock
					lock(queue->jobqueue);

				// Did we win the lottery?
				if (!lock->terminate_mcguffins.empty())
				{
					lock->terminate_mcguffins.pop_front();
					return;
				}

				if (lock->pending_jobs.empty())
				{
					lock.wait();
					continue;
				}

				ref<jobObj>
					j=lock->pending_jobs.front();
				lock->pending_jobs.pop_front();
				j;
			});

		// Save the current signal mask.
		sigset current=sigset::current();

		// Set the job's signal mask, and execute it.
		job->sigmask.setmask();
		try {
			job->job(this);
		} catch (const exception &e)
		{
			captured_exception(e);
		} catch (...) {
			captured_unknown_exception();
		}

		// Restore the signal mask, mark the job as completed.
		current.setmask();

		typename jobqueue_t::lock lock(queue->jobqueue);

		--lock->unprocessed_jobs;
		lock.notify_all();
	}
}

//! Add a run() job to the queue of jobs to execute

//! \internal
//!
//! Adjust worker threads, as necessary.
template<typename workerThreadType>
void workerpoolObj<workerThreadType>::addJob(//! The job to execute, ASAP
					     const ref<jobObj> &job)
{
	sigset::block_all block_all_signals;

	// Get thread parameters, make them sane.

	size_t minthreads=minthreadcount_prop.get();
	size_t maxthreads=maxthreadcount_prop.get();

	if (minthreads < 1)
		minthreads=1;
	if (maxthreads < minthreads)
		maxthreads=minthreads;

	typename jobqueue_t::lock lock(jobqueue->jobqueue);

	// Add a new worker thread if the current number of worker threads is
	// less than the maximum, and:
	//
	// - The number of worker threads is less than the minimum, or
	// - The number of unprocessed jobs is at least as many as the number
	// - of worker threads that were started.

	while (lock->threadcount_started < maxthreads &&
	       (lock->threadcount_started < minthreads ||
		lock->unprocessed_jobs >= lock->threadcount_started))
	{
		workers->push_back(LIBCXX_NAMESPACE::run
				   (ref<workerObj>::create(threadnames_prop
							   .get()),
				    jobqueue));
		++lock->threadcount_started;
	}

	lock->pending_jobs.push_back(job);
	++lock->unprocessed_jobs;
	lock.notify_all();

	// Cancel worker threads that are idle.

	while (lock->threadcount_started > minthreads &&
	       lock->threadcount_started > lock->unprocessed_jobs)
	{
		lock->stop1();
		lock.notify_all();
	}
}

#if 0
{
#endif
}
#endif
