/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_runthreadbaseobj_H
#define x_runthreadbaseobj_H

#include <x/namespace.h>
#include <x/ref.H>
#include <x/obj.H>
#include <x/threads/runfwd.H>
#include <thread>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! An internal cleanup thread that joins all threads that invoked run()

//! \internal

void cleanup_thread_func() LIBCXX_HIDDEN;

//! Base class of runthreadObj<T>

//! The first constructed instance of runthreadbaseObj starts a cleanup thread.
//! The cleanup thread invokes join() for threads started by run().
//!
//! A destructor callback that's attached to each thread's mcguffin places an
//! instance of this class on the cleanup thread's queue. The cleanup thread
//! drains the queue, joins the terminated thread, the invokes the joined()
//! method, which gets implemented in the runthreadObj<T> subclass.

class runthreadbaseObj : virtual public obj {

protected:

	//! Constructor
	runthreadbaseObj();

public:
	//! Destructor
	~runthreadbaseObj() noexcept;

private:

	//! A destructor callback attached to a mcguffin passed to the thread.

	class threadCleanupObj : virtual public obj {

		//! When the thread terminates, send this to the cleanup thread.
		ref<runthreadbaseObj> thr;

		//! Whether this thread cleanup object is "accounted" for.

		//! The accounting is used to keep track of the running threads.
		//! When main() returns, and global destructors get called,
		//! if something is still running, a global destructor waits for
		//! all threads to stop.
		//!
		//! The constructor increments the global counter, the
		//! destructor decrements it. To prevent a rare exception from
		//! throwing the counter off, this gets set only if the
		//! internal counter got incremented.
		bool accounted_for;

	public:
		//! The constructor saves the thread object

		threadCleanupObj(const ref<runthreadbaseObj> &thrArg);

		//! Destructor
		~threadCleanupObj() noexcept LIBCXX_HIDDEN;

		//! Thread terminated.

		void destroyed();
	};

	//! The joinable thread

	std::thread thr;

	//! Its ID

	std::thread::id thr_id;

	//! joined() gets invoked by the cleanup thread after joining the terminated thread

	virtual void joined() noexcept=0;
public:

	friend void cleanup_thread_func();

	friend class run_async;
	friend class threadCleanupObj;

	//! Return \c true if the thread has terminated, \c false otherwise.

	//! A return of \c true means that the thread which invokes the thread
	//! object's %run() method terminated and was joined.

	virtual bool terminated() const=0;

	//! Wait for the thread to terminate

	//! After returning from wait(), terminate() will be true.

	virtual void wait()=0;

	//! Return the running thread's ID

	std::thread::id get_id();
};

#if 0
{
#endif
}
#endif
