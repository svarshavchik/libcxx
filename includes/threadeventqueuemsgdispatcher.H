/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_threadeventqueuemsgdispatcher_H
#define x_threadeventqueuemsgdispatcher_H

#include <x/msgdispatcher.H>
#include <x/eventqueue.H>
#include <x/mcguffinref.H>
#include <x/threads/run.H>
#include <x/threads/runthreadsingleton.H>
#include <x/threadeventqueuemsgdispatcherfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! An event file descriptor queue-based message dispatcher, tied to a thread.

//! \see threadeventqueuemsgdispatcher

template<typename objClass>
class threadeventqueuemsgdispatcherObj: public msgdispatcherObj,
					virtual public runthreadsingleton {

protected:

	//! The type of the queue that holds the request messages

	typedef eventqueue<dispatchablebase> msgqueue_t;

	//! The queue for the request messages.

	//! The subclass should not use reference it, but only use the
	//! parameter to its run(). The executing thread in the subclass
	//! can only reference this object after run_thread() returns in the
	//! parent thread. This synchronization must be handled externally
	//! (run_thread() may not necessarily return, in the parent thread,
	//! before run() gets invoked in the new thread).

	mcguffinref<eventqueueObj<dispatchablebase>> msgqueue_ref;

public:
	//! Constructor

	threadeventqueuemsgdispatcherObj()
		: msgqueue_ref(mcguffinref<eventqueueObj<dispatchablebase>>
			       ::create())
	{
	}

	//! Default destructor
	~threadeventqueuemsgdispatcherObj() noexcept
	{
	}

	//! Execute run_thread() constructing a message queue using a given eventfd

	//! (it's actually the other way around).

	template<typename ...Args>
	auto run_thread_using_eventfd(const eventfd &eventfdArg,
				      Args && ...args)
		-> decltype(run(ref<objClass>(0),
				ref<obj>::create(),
				msgqueue_t::create(eventfdArg),
				std::forward<Args>(args)...))
	{
		auto mcguffin=ref<obj>::create();

		auto msgqueue=msgqueue_t::create(eventfdArg);

		// We call run, which will throw an exception if the thread
		// is already running. Only afterwards to we install the
		// mcguffin-based reference.

		auto p=dynamic_cast<objClass *>(this);

		if (!p)
			invalid_cast(typeid(*this).name(),
				     typeid(objClass).name());

		auto ret=run(ref<objClass>(p),
			     mcguffin,
			     msgqueue, std::forward<Args>(args)...);

		msgqueue_ref->install(msgqueue, mcguffin);
		return ret;
	}

	//! Execute run() in a new thread.

	template<typename ...Args>
	auto run_thread(Args && ...args)
		-> decltype(this->run_thread_using_eventfd(eventfd::create(),
						     std::forward<Args>(args)...
						     ))
	{
		return run_thread_using_eventfd(eventfd::create(),
						std::forward<Args>(args)...);
	}

protected:

	//! Implement event()

	//! If the executing thread is already gon, the message gets
	//! quietly discarded.

	void event(//! A reference to the request message
		   const dispatchablebase &msg)
	{
		auto p=msgqueue_ref->getptr();

		if (!p.null())
			p->event(msg);
	}
};

#if 0
{
#endif
}
#endif
