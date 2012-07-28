/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventqueue_H
#define x_eventqueue_H

#include <x/eventqueueobj.H>
#include <x/ref.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A thread-safe event queue

//! \code
//! typedef eventqueue<myClass> queue_t;
//!
//! queue_t q(queue_t::create(myEventFd));
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
//! \note
//! This is a template alias for a \ref ref "ref" to an eventqueueObj.

template<typename argType>
#ifdef DOXYGEN
class eventqueue {}; // TODO
#else
using eventqueue=ref< eventqueueObj<argType> >;
#endif

//! A nullable reference pointer to the queue object.

//! \see eventqueue

template<typename argType>
#ifdef DOXYGEN
class eventqueue {};
#else
using eventqueueptr=ptr< eventqueueObj<argType> >;
#endif

#if 0
{
#endif
}
#endif
