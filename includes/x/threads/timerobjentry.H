/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_timerobjentry_H
#define x_timerobjentry_H

#include <x/namespace.h>
#include <x/timespec.H>
#include <x/threads/timerobj.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! This timer task is owned by a timer thread

//! This object gets installed into a timertaskObj when it is scheduled by
//! a timer thread. The timertaskobj holds a weak reference on it. The reference
//! is owned by the timer thread. When the timer thread drops the task,
//! it drops its reference to this object, and the weak reference in the
//! timer object goes out of scope automatically.
//!
//! This object contains a (weak) reference to the timer thread's message queue.
//! This allows the timer task to send a request to cancel itself from the
//! timer thread, if it succesfully unweak-s it. The message itself consists of
//! a weak reference to this object, so if the timer thread concurrently drops
//! the task, by the time the timer thread picks up the message, the weak
//! reference in the message will be null.

class timerobjentryObj : virtual public obj {

public:
	//! The message queue to the timer thread that owns this task
	weakptr<ptr<timerObj::msgqObj> > timerq;

	//! The internal iterator in the timer thread's queue

	//! Initialized by the timer thread.
	//!
	timerObj::tasks_t::iterator installed_iter;

	timerobjentryObj() LIBCXX_HIDDEN {}
	~timerobjentryObj() LIBCXX_HIDDEN {}
};

//! A reference to a timer entry object.

typedef ref<timerobjentryObj> timerobjentry;

//! A nullable pointer reference to a timer entry object.

typedef ptr<timerobjentryObj> timerobjentryptr;

#if 0
{
#endif
}
#endif
