/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_timertaskobj_H
#define x_timertaskobj_H

#include <x/threads/timertaskentryfwd.H>
#include <x/ptrfwd.H>
#include <x/weakptrfwd.H>
#include <x/weakptrobj.H>
#include <x/obj.H>
#include <x/namespace.h>

#include <map>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class timerObj;
class timerobjentryObj;

class canceltimertaskObj;

//! A regularly-scheduled task

//! \see ::timer
//!
//! A task that's executed by a ::timer. The subclass implements the run()
//! method. The ::timer object invokes the run() method when this task's time
//! to execute comes up.
//!
//! \note
//! A single task can be scheduled for execution only by one timer
//! object.
//! Attempting to schedule the same task object by another timer throws
//! an %exception. Different instances of the task object may be
//! scheduled for execution by different timers.

class timertaskObj : virtual public obj {

	//! The timer thread's entry for this task

	weakptr<timertaskentryptr> timerentry;

	//! Bind this task to a thread

	bool install(const timertaskentry &) LIBCXX_HIDDEN;

public:
	friend class timerObj;

	//! Default constructor
	timertaskObj();

	//! Default destructor
	~timertaskObj();

	//! Cancel this task

	//! The running task may invoke its own cancel()
	//! method. The cancel() method returns, and the task continues to run.
	//! When the task finishes and returns,
	//! the task is stopped from further execution.
	//!
	//! A task's cancel() method may also be invoked by another thread.
	//! If the task is not currently executing, it's stopped from being
	//! executed any more. If the task is currently running, cancel()
	//! waits until it finishes, then removes it.

	void cancel();

	//! Return a mcguffin that cancels this timer task

	ref<canceltimertaskObj> autocancel();

	//! The task that gets executed.

	virtual void run()=0;
};

//! The mcguffin that's returned by timertaskObj::autocancel().

class canceltimertaskObj : virtual public obj {

	//! The task to cancel
	ref<timertaskObj> task;

public:

	//! Constructor
	canceltimertaskObj(const ref<timertaskObj> &taskArg);

	//! Move constructor
	canceltimertaskObj(ref<timertaskObj> &&taskArg);

	//! The destructor invokes cancel()
	~canceltimertaskObj();
};

#if 0
{
#endif
}
#endif
