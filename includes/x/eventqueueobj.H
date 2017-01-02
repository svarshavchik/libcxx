/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventqueueobj_H
#define x_eventqueueobj_H

#include <x/obj.H>
#include <x/eventhandlerobj.H>
#include <x/eventfd.H>

#include <queue>

#include <x/namespace.h>


namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! An event queue implementation

//! This is a reference-counted object that should not be used directly.
//! \see eventqueue

template<typename argType>
class eventqueueObj : public eventhandlerObj<argType> {

	//! The associated event file descriptor

	eventfd fd;

	//! The queue object

	std::queue<argType> queue;

public:
	//! Initial constructor
	eventqueueObj(const eventfd &fdArg=eventfd::create()) noexcept
	: fd(fdArg) {}

	//! Default destructor
	~eventqueueObj() {}

	//! Retrieve the associated event handler

	eventfd getEventfd() noexcept { return fd; }

	//! Put a new event into the queue

	void event(//! The new event
		   const argType &arg)
	{
		std::lock_guard<std::mutex> lock(this->objmutex);

		queue.push(arg);
		try {
			fd->event(1);
		} catch (...)
		{
		}
	}

	//! Check if the queue is empty

	//! \return \c true if there's nothing in the queue
	bool empty()
	{
		std::lock_guard<std::mutex> lock(this->objmutex);

		return queue.empty();
	}

	//! Retrieve the next object from the queue

	//! \return the next object, which is removed from the queue. If the
	//! queue is empty, pop() waits on the file descriptor.

	argType pop()
	{
		while (1)
		{
			{
				std::lock_guard<std::mutex>
					lock(this->objmutex);

				if (!queue.empty())
				{
					argType arg(queue.front());

					queue.pop();

					return arg;
				}
			}

			fd->event();
		}
	}
};

#if 0
{
#endif
}
#endif
