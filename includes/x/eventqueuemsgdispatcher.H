/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventqueuethreadmsgdispatcher_H
#define x_eventqueuethreadmsgdispatcher_H

#include <x/msgdispatcher.H>
#include <x/eventqueue.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

#if LIBCXX_INTERNAL_BUILD
#else
#warning "Deprecated - use threadmsgdispatcher.H"
#endif

//! An event file descriptor queue-based message dispatcher

//! Instantiates an \ref eventqueue "eventqueue" and implements
//! event() by putting messages into the event queue.

class eventqueuemsgdispatcherObj : public msgdispatcherObj {

protected:

	//! The type of the queue that holds the request messages
	typedef eventqueue<dispatchablebase> msgqueue_t;

	//! A queue for the request messages.

	//! The subclass will want to pop the queue, to process requests.
	//!
	msgqueue_t msgqueue;

public:
	//! Constructor

	eventqueuemsgdispatcherObj(//! The event file descriptor used by the request event queue.

			 //! We'll be happy to create a new one, by default
			 //!
			 const eventfd &eventfd=eventfd::create());

	//! Default destructor
	~eventqueuemsgdispatcherObj();

protected:

	//! Implement event()

	void event(//! A reference to the request message
		   const dispatchablebase &msg) override;
};

#if 0
{
#endif
}
#endif
