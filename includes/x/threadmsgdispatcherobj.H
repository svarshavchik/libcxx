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

//! A thread object, with a message-based interface.

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
//!     mcguffin=nullptr;
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
//! auto t=INSERT_LIBX_NAMESPACE::start_threadmsgdispatcher(thr, 1, 2);
//!
//! \endcode
//!
//! This class implements the following design pattern:
//!
//! * An object that runs a message-based execution thread.
//!
//! * Public methods send messages to the execution thread.
//!
//! * The messages sent to the execution thread have the result of the
//! object's private methods getting invoked by an execution thread.
//!
//! \par The message queue
//!
//! The internal queue for the executing thread is
//! an \ref eventqueue "event file descriptor-based queue". The execution
//! thread owns the queue, and the
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
//! \ref start_threadmsgdispatcher "INSERT_LIBX_NAMESPACE::start_threadmsgdispatcher()" must be used to
//! start the execution thread. start_threadmsgdispatcher() does not return until the
//! execution thread is running.
//!
//! start_threadmsgdispatcher() returns the value from \ref run "INSERT_LIBX_NAMESPACE::run()"
//! that started the execution thread.
//!
//! \par Execution thread startup sequence
//!
//! The started thread invokes the thread object's run() method.
//! The parameters to run() are.
//!
//! * A mcguffin (see below).
//!
//! * Any additional parameters are forwarded from start_threadmsgdispatcher().
//!
//! The execution thread must take the following course of action:
//!
//! * Construct a \c msgqueue_auto object in automatic scope, on the stack.
//!
//! * After the \c msgqueue_auto object gets constructed, destroy the
//! mcguffin by setting it to a \c nullptr.
//!
//! The parent thread does not return from start_threadmsgdispatcher() until the mcguffin
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
//! gets created.
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
//! \par This is a stoppable singleton thread
//!
//! \c threadmsgdispatcherObj inherits from \c stoppableObj, and has a
//! stop() method that sends a message to the execution thread to throw a
//! \c INSERT_LIBX_NAMESPACE::stopexception.
//!
//! \c threadmsgdispatcherObj also inherits from \ref runthreadsingleton,
//! limiting it to one execution thread per object, because the execution
//! thread retains ownership of the message event queue.
//!
//! \par Auxiliary queues
//!
//! \code
//! class myThreadObj : public INSERT_LIBX_NAMESPACE::threadmsgdispatcherObj {
//!
//! public:
//!
//!       active_queue_t second_queue;
//!
//!       void run(INSERT_LIBX_NAMESPACE::ptr<INSERT_LIBX_NAMESPACE::obj> &mcguffin,
//!                int param1, int param2);
//!
//!       void queue_notifications(int n);
//!
//!       void send_notifications();
//!
//!       // ...
//!
//! private:
//!
//!       void event_handler(int n);
//! };
//!
//! void myThreadObj::run(INSERT_LIBX_NAMESPACE::ptr<INSERT_LIBX_NAMESPACE::obj> &mcguffin);
//! {
//!     msgqueue_auto q{this, INSERT_LIBX_NAMESPACE::eventfd::create() };
//!     msgqueue_auto q2{this, second_queue};
//!
//! // ...
//! }
//!
//! void myThreadObj::queue_notifications(int n)
//! {
//!     sendeventaux(this->second_queue, &myThread::event_handler, this, n);
//! }
//!
//! void myThreadObj::send_notifications()
//! {
//!     process_events(second_queue);
//! }
//! \endcode
//!
//! It is possible to create one or more, "auxiliary" message queues.
//! sendeventaux() takes an extra parameter, a reference to a \c active_queue_t
//! which should be a class member. The message does not get processed by the
//! execution thread's main message dispatching loop, but gets placed on the
//! queue.
//!
//! The execution thread's \c run() method should instantiate an additional
//! \c msgqueue_auto, passing a reference to the auxiliary message queue,
//! before destroying its mcguffin. The auxiliary message queue has the same
//! lifetime scope as the main thread message dispatching queue, and goes out
//! of scope, and gets destroyed releasing all references to any unprocessed
//! messages, upon thread termination, with \c sendeventaux() taking no
//! action if the execution thread is not running, and the auxiliary message
//! queue gets destroyed.
//!
//! Messages added to the auxiliary message queue do not get processed until
//! \c process_events() gets called, which moves them to the main
//! message queue, where they should get processed by the execution thread.

class threadmsgdispatcherObj : public stoppableObj,
			       virtual public runthreadsingleton {

public:

	//! The queue for the messages.

	typedef eventqueue<dispatchablebase> msgqueue_t;

	//! Nullable pointer to the message queue
	typedef eventqueueptr<dispatchablebase> msgqueueptr_t;

	//! A mutex-protected active queue.

	typedef mpobj<msgqueueptr_t> active_queue_t;
private:

	//! Default message queue
	active_queue_t msgqueueptr;

public:

	//! Return a mutex-protected queue

	//! A null ptr gets returned if the execution thread is not running.

	static msgqueueptr_t get_msgqueue(active_queue_t &q)
	{
		typename mpobj<msgqueueptr_t>::lock lock(q);

		return *lock;
	}

	//! Return the default message queue

	//! A null ptr gets returned if the execution thread is not running.

	msgqueueptr_t get_msgqueue()
	{
		return get_msgqueue(msgqueueptr);
	}

	//! Message queue for the executing thread to read.

	//! The executing thread constructs it on the stack.

	class msgqueue_auto : public msgqueue_t {

		//! The queue pointer in the thread dispatcher object.

		active_queue_t &active_queue;

	public:

		using msgqueue_t::operator*;
		using msgqueue_t::operator->;

		//! Deleted operator

		msgqueue_auto(const msgqueue_auto &)=delete;

		//! Deleted operator

		msgqueue_auto &operator=(const msgqueue_auto &)=delete;

		//! Construct the message queue.

		msgqueue_auto(threadmsgdispatcherObj *me,
			      const eventfd &fdArg=eventfd::create());

		//! Construct the message queue.

		msgqueue_auto(const ref<threadmsgdispatcherObj> &me,
			      const eventfd &fdArg=eventfd::create());

		//! Construct an auxiliary queue

		msgqueue_auto(threadmsgdispatcherObj *me,
			      active_queue_t &auxqueue);

		//! Construct an auxiliary queue

		msgqueue_auto(const ref<threadmsgdispatcherObj> &me,
			      active_queue_t &auxqueue);

		//! Construct a queue
		msgqueue_auto(const msgqueue_t &q,
			      active_queue_t &active_queue);

		//! Destroy the message queue

		~msgqueue_auto();

		//! Process the next event.

		void event();
	};

	//! msgqueue_auto as a reference-counted object.

	class msgqueue_objObj : public msgqueue_auto, virtual public obj {

	public:
		msgqueue_objObj(const ref<threadmsgdispatcherObj> &me,
				const eventfd &fdArg=eventfd::create());
		~msgqueue_objObj();
	};

	//! A msgqueue_auto object.

	//! Makes it possible to construct a queue in advance of the
	//! formal start_threadmsgdispatcher.

	typedef ref<msgqueue_objObj> msgqueue_obj;

	//! A message to the execution thread.

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

		~msgObj() = default;
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
		return sendeventaux(msgqueueptr,
				    method,
				    std::forward<me_type>(me),
				    std::forward<Parameters>(parameters)...);
	}

	//! Add a message to an auxiliary queue

	template<typename method_type,
		 typename me_type,
		 typename ...Parameters>
	static void sendeventaux(//! Auxiliary queue
				 active_queue_t &auxqueue,

				 //! Must be a class method
				 method_type method,

				 //! A pointer to \c this, the method's class
				 me_type &&me,

				 //! Any parameters to pass to the method
				 Parameters && ...parameters)
	{
		auto q=get_msgqueue(auxqueue);

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

	//! Transfer events from an auxiliary queue into this one.

	void process_events(active_queue_t &auxqueue);

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
