/*
** Copyright 2016 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_threadmsgdispatcherobj_H
#define x_threadmsgdispatcherobj_H

#include <x/eventqueuefwd.H>
#include <x/eventqueueobj.H>
#include <x/stopexception.H>
#include <x/stoppableobj.H>
#include <x/threads/runthreadsingleton.H>
#include <x/mpobj.H>
#include <x/dispatchablebase.H>
#include <functional>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A thread object, with a message interface.

//! \code
//! class myThreadObj : public INSERT_LIBX_NAMESPACE::threadmsgdispatcherObj {
//!
//! public:
//!
//!       void run(INSERT_LIBX_NAMESPACE::ptr<INSERT_LIBX_NAMESPACE::obj> &mcguffin,
//!                int param1, int param2);
//!
//!       void notify(int n)
//!       {
//!             sendevent(&myThread::event_handler, this, n);
//!       }
//!
//! private:
//!
//!       void event_handler(int n);
//! };
//!
//! void myThreadObj::run(INSERT_LIBX_NAMESPACE::ptr<INSERT_LIBX_NAMESPACE::obj> &mcguffin,
//!                      int param1, int param2);
//! {
//!     msgqueue_auto q{this, INSERT_LIBX_NAMESPACE::eventfd::create() };
//!
//!     mcguffin=INSERT_LIBX_NAMESPACE::ptr<INSERT_LIBX_NAMESPACE::obj>();
//!
//!	try {
//!		while (1)
//!			q.event();
//!	} catch (const INSERT_LIBX_NAMESPACE::stopexception &e)
//!	{
//!	} catch (const INSERT_LIBX_NAMESPACE::exception &e)
//!	{
//!		std::cerr << e << std::endl;
//!		exit(1);
//!	}
//! }
//!
//! auto thr=INSERT_LIBX_NAMESPACE::ref<mythreadObj>::create();
//!
//! auto t=INSERT_LIBX_NAMESPACE::start_thread(thr, 1, 2);
//!
//! \endcode
//!
//! This class implements the following design pattern:
//!
//! * An object that runs an message-based execution thread.
//!
//! * Public method functions send messages to the execution thread.
//!
//! * The messages send to the execution thread result in the object's
//! private methods getting invoked by an execution thread.
//!
//! \par The message queue
//!
//! The internal queue that sends messages to the executing thread is
//! an \ref eventqueue "event file descriptor-based queue". The message
//! queue remains in existence as long as the execution thread is running.
//! When the execution thread terminates the queue gets destroyed. Any
//! unprocessed messages in the queue get destroyed.
//!
//! \par Sending messages to the execution thread
//!
//! sendevent() sends a message to the object's thread. The parameters to
//! sendevent() are:
//!
//! * A pointer to a class method
//!
//! * \c this object pointer.
//!
//! * Any remaining parameters get copied by value, and a copy of these
//! parameters get passed to the class method.
//!
//! \c sendevent() returns without sending a message after the execution thread
//! stops. The caller can detect this condition by passing a
//! mcguffin for a parameter. Note that the mcguffin gets destroyed when
//! the message gets processed, and the private thread method returns
//! (unless the private method stashed away its own reference to the
//! mcguffin).
//!
//! \par Starting the execution thread.
//!
//! \ref start_thread "INSERT_LIBX_NAMESPACE::start_thread()" must be used to
//! start the execution thread. start_thread() does not return until the
//! execution thread is running.
//!
//! start_thread() returns the value from \ref run "INSERT_LIBX_NAMESPACE::run()"
//! that started the execution thread.
//!
//! \par Execution thread startup sequence
//!
//! The started thread invokes the thread object's run() method.
//! The parameters to run() are.
//!
//! * A mcguffin pointer.
//!
//! * Any additional parameters are forwarded from start_thread().
//!
//! The execution thread must take the following course of action:
//!
//! * Construct a \c msgqueue_auto object in automatic scope, on the stack.
//!
//! * After the \c msgqueue_auto object gets constructed, destroy the
//! mcguffin by setting it to an empty \c ptr.
//!
//! The parent thread will not return from start_thread() until the mcguffin
//! gets destroyed.
//!
//! \par The event queue
//!
//! \c msgqueue_auto is a subclass of an \ref eventqueue "INSERT_LIBX_NAMESPACE::eventqueue",
//! an event file descriptor-based queue. Its constructor takes the
//! following parameters:
//!
//! * \c this object, specifically \c INSERT_LIBX_NAMESPACE::threadmsgdispatcherObj
//!
//! * An optional parameter specifying the \ref eventfd "event file descriptor"
//! that runs the message queue. If not specified, a new file descriptor
//! gets
//!
//! \c msqueue_auto's `event()` method removes the next message from the
//! message queue, and invokes the class method that was sent by the message.
//! event() blocks and waits for a message, if the message queue is empty.
//!
//! \code
//!    if (q->empty())
//!    {
//!          INSERT_LIBX_NAMESPACE::eventfd fd=q->getEventfd();
//! \endcode
//!
//! Because \c msgqueue_auto is a subclass of an \ref eventqueue "event queue",
//! its methods are available to the execution thread.
//!
//! \par This is a stoppable thread
//!
//! \c threadmsgdispatcherObj inherits from \c stoppableObj, and has a
//! stop() method that sends a message to the execution thread to throw a
//! \c INSERT_LIBX_NAMESPACE::stopexception.

class threadmsgdispatcherObj : public stoppableObj,
			       public runthreadsingleton {

public:

	//! The queue for the messages.

	typedef eventqueue<dispatchablebase> msgqueue_t;

private:

	//! Nullable pointer to the message queue
	typedef eventqueueptr<dispatchablebase> msgqueueptr_t;

	//! Mutex protected message queue

	mpobj<msgqueueptr_t> msgqueueptr;

protected:

	//! Return the message queue
	auto get_msgqueue()
	{
		typename mpobj<msgqueueptr_t>::lock lock(msgqueueptr);

		return *lock;
	}

	//! Message queue for the executing thread to read.

	class msgqueue_auto : public msgqueue_t {

		//! The executing thread.
		threadmsgdispatcherObj *me;

	public:

		using msgqueue_t::operator*;
		using msgqueue_t::operator->;

		//! Construct the message queue.

		msgqueue_auto(threadmsgdispatcherObj *me,
			      const eventfd &fdArg=eventfd::create())
			: msgqueue_t(msgqueue_t::create(fdArg)),
			  me(me)
		{
			// Install it in mgsqueueptr

			typename mpobj<msgqueueptr_t>::lock
				lock(me->msgqueueptr);

			*lock= static_cast<msgqueue_t>(*this);
		}

		//! Destroy the message queue

		~msgqueue_auto() noexcept
		{
			// Remove myself from msgqueueptr
			typename mpobj<msgqueueptr_t>::lock
				lock(me->msgqueueptr);

			*lock=msgqueueptr_t();
		}

		//! Process the next event.

		void event();
	};

	//! Message to the execution thread.

	//! Captures the method to invoke, and this object, and any
	//! method parameters.

	template<typename bind_type> class msgObj : public dispatchablebaseObj {

	public:
		//! The return value from std::bind

		bind_type method_call;

		//! Emplace the bounded method call.
		template<typename method_type,
			 typename me_type,
			 typename ...Args>
		msgObj(method_type method, me_type *me,
		       Args && ...args)
			: method_call(std::bind(method,
						me,
						std::forward<Args>(args)...))
		{
		}

		//! Implement dispatch()

		void dispatch() override
		{
			method_call();
		}

		//! Destructor

		~msgObj() noexcept = default;
	};

public:
	//! Send a message to the execution thread.
	template<typename method_type,
		 typename me_type,
		 typename ...Parameters>
	void sendevent(//! Must be a class method
		       method_type method,

		       //! A pointer to \c this, the method's class
		       me_type &&me,

		       //! Any parameters to pass to the method
		       Parameters && ...parameters)
	{
		auto q=get_msgqueue();

		if (q.null())
			return; // Already destroyed.

		typedef decltype(std::bind(method, &*me,
					   std::forward<Parameters>
					   (parameters)...)) bind_t;

		q->event(ref<msgObj<bind_t>>
			 ::create(method,
				  &*me,
				  std::forward<Parameters>(parameters)...));
	}

private:
	//! Throw an exception to stop this thread.

	static void stop_me(threadmsgdispatcherObj *me) LIBCXX_HIDDEN;
public:

	//! stop the executing thread.
	void stop() override;
};

#if 0
{
#endif
}
#endif
