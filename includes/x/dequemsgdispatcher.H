/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_dequethreadmsgdispatcher_H
#define x_dequethreadmsgdispatcher_H

#include <x/msgdispatcher.H>
#include <x/mpobj.H>
#include <deque>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

#if LIBCXX_INTERNAL_BUILD
#else
#warning "Deprecated - use threadmsgdispatcher.H"
#endif

//! An old-fashioned deque-based message dispatching queue.

//! Implementation of event() by stuffing the dispatchable message into a
//! dequeue. Exposes the dequeue as a mutex-protected object.

class dequemsgdispatcherObj : public msgdispatcherObj {

protected:

	//! The type of the queue that holds the request messages

	typedef mpcobj<std::deque<dispatchablebase> > msgqueue_t;

	//! The queue for the request messages.

	msgqueue_t msgqueue;

public:
	//! Constructor

	dequemsgdispatcherObj();

	//! Default destructor
	~dequemsgdispatcherObj();

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
