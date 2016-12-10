/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_msgdispatcher_H
#define x_msgdispatcher_H

#include <x/obj.H>
#include <x/ptr.H>
#include <x/stoppable.H>
#include <x/stopexception.H>
#include <x/dispatchablebase.H>
#include <x/dispatchablemsgobj.H>

#include <type_traits>

#warning "Deprecated - use threadmsgdispatcher.H"

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Thread message dispatcher

//! This framework implements a thread that receives messages from other
//! threads, and acts on them in some way.
//!
//! Define a subclass that derives from this class.
//!
//! Define messages as subclasses of msgbaseObj which implement
//! msgbaseObj::dispatch().
//!
//! Submit messages to the class by invoking event().
//!
//! The subclass's run() main loop does something like this:
//!
//! \code
//! while (1)
//! {
//!    msgqueue->pop()->dispatch();
//! }
//! \endcode
//!
//! Invoking stop() sends a message that results in dispatch() throwing
//! a \ref stopexception "stopexception".

class msgdispatcherObj : public stoppableObj {

public:
	//! Constructor

	msgdispatcherObj();

	//! Default destructor
	~msgdispatcherObj() noexcept;

protected:

	//! Queue up a request
	virtual void event(//! A reference to the request message
			   const dispatchablebase &msg)=0;

	//! Helper function for sending an object directly

	//! Invoke from the dispatching subclass. The subclass defines a
	//! public method. The public method invokes
	//! sendevent<msg>(this, args...). The first argument is \c this,
	//! any remaining optional arguments get forwarded to msg's constructor.
	//! \c msg is the template parameter class.
	//!
	//! This method constructs a msgObj, and invokes event().
	//!
	//! Define a subclass that looks something like this:
	//!
	//! \code
	//!
	//! class myClass : public runnableObj, public msgdispatcherObj {
	//!
	//! private:
	//!
	//!   void dispatch(const mystatus &msg);
	//!
	//!   void dispatch(const myterminate &msg);
	//!
	//! public:
        //!   friend class msgdispatcherObj;
	//!
	//! // ...
	//!
	//!   void sendstatus(bool flag, const std::string &arg)
	//!   {
	//!       sendevent<mystatus>(this, flag, arg);
	//!   }
	//!
	//!   void sendterminate()
	//!   {
	//!       sendevent<myterminate>(this);
	//!   }
	//!
	//!   void run(ptr<obj> &start_arg, const thread &threadArg)
	//!   {
	//!       while (1)
	//!       {
	//!           msgqueue->pop()->dispatch();
	//!       }
	//!   }
	//! };
	//! \endcode
	//!
	//! sendstatus() and sendterminate() submit a message to the execution
	//! thread. The friend declaration permits the classes defined in
	//! msgdispatcherObj to invoke the private dispatch() method, enforcing
	//! the requirement that dispatch() may only be invoked from the
	//! message dispatcher's run() loop.
	//!
	//! \note
	//! dispatch() may be defined with non-const arguments, because the
	//! object gets constructed on the queue, and the parameters to the
	//! public function get copied into it.

	template<typename msg_type, typename dispatcher_type,
		 typename ...Args>
	void sendevent(//! The subclass passes \c this here
		       const dispatcher_type &dispatcher,

		       //! Any arguments to forward to obj__type's constructor
		       Args && ...args)
	{
		event(make_dispatchablemsg<msg_type>
		      (dispatcher, std::forward<Args>(args)...));
	}

	//! Helper function for sending an object directly

	//! \overload
	template<typename msg_type, typename dispatcher_type,
		 typename ...Args>
	void sendevent(//! The subclass passes \c this here
		       dispatcher_type && dispatcher,

		       //! Any arguments to forward to obj__type's constructor
		       Args && ...args)
	{
		event(make_dispatchablemsg<msg_type>
		      (std::forward<dispatcher_type>(dispatcher),
		       std::forward<Args>(args)...));
	}

private:

	//! A message that throws a stopexception in the context of the thread

	class throwstopexceptionObj;

public:
	//! Queue up a message that throws an %exception, when it gets dispatched.

	void stop();
};

#if 0
{
#endif
}
#endif
