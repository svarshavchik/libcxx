/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventqueuefwd_H
#define x_eventqueuefwd_H

#include <x/ptrfwd.H>

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename argType>
class eventqueueObj;

//! A thread-safe event queue

//! \code
//! typedef eventqueue<myClass> queue_t;
//!
//! queue_t q{queue_t::create(myEventFd)};
//!
//! q->event(myClassInstance);
//!
//! if (!q->empty())
//! {
//!     doSomething(q->pop());
//! }
//! \endcode
//!
//! This template defines a thread-safe event queue. The
//! \ref eventqueue "eventqueue" itself is a reference-counted object.
//!
//! The queue uses an eventfd for signaling. The eventfd is passed to the
//! create() function. If omitted, a new event file descriptor gets created.
//!
//! \link eventqueueObj::empty() empty() \endlink returns true if there are
//! no objects in the queue.
//! \link eventqueueObj::event() event() \endlink adds an object to the queue,
//! \link eventqueueObj::pop() pop() \endlink removes it, if the queue is
//! empty, pop() waits on the event file descriptor, and tries again.
//!
//! \note
//! The underlying queue object is a subclass of an
//! \ref eventfactory "event factory handler". The queue reference can
//! be passed directly to the
//! \link eventfactoryObj::registerHandler() registerHandler() \endlink
//! method.
//!
//! \code
//! queue_t qq{queue_t::create()};
//!
//! q->transfer_events(qq);
//! \endcode
//!
//! transfer_events() transfers any unprocessed events from another queue
//! into this queue.
//!
//! This acquires an internal lock on the other queue first, then this queue.

template<typename argType>
using eventqueue=ref< eventqueueObj<argType> >;

//! A nullable reference pointer to the queue object.

//! \see eventqueue

template<typename argType>
using eventqueueptr=ptr< eventqueueObj<argType> >;

#if 0
{
#endif
}
#endif
