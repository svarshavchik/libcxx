/*
** Copyright 2012-2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_vipobj_H
#define x_vipobj_H

#include <x/namespace.h>
#include <x/sipobj.H>
#include <x/eventcallback.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A Very Important object

//! Define a template for a Very ImPortant object: an object, and a list
//! of registered callbacks, with consistent semantics for notifying the
//! registered callbacks when the Very ImPortant object changes.
//!
//! The Very Important object is a further subclass of a
//! \ref sipobj "Somewhat Important Object", that supplements its
//! read-write lock access mechanism with two additional locks: a handler lock
//! and an update lock, that implements a callback-type facility.
//!
//! When a new callback gets registered, it gets immediately invoked and
//! given the curren tvalue of the Very ImPortant object, then subsequent
//! changes to the objects are reported to the callback, until the callback
//! goes out of scope.
//!
//! The callbacks are reference-counted objects, and
//! the list of registered callbacks is implemented by
//! an \ref eventcallback "eventcallback" template, which holds weak
//! references. The callbacks remain registered and receive notifications
//! of updates to the value of the Very ImPortant objects until they go out
//! of scope, and get destroyed.
//!
//! Careful locking is used to provide a consistent reporting of changes to
//! the value of the object in a multithread context. There's also a concept
//! of obtaining write access to the object, without reporting the write
//! access to the registered callbacks.
//!
//! The first template parameter specifies the Very ImPortant object class.
//! The second parameter specifies the reference-counted handler
//! class, that must be derived from \ref obj "obj". The class must define
//! a member type, arg_t, that defines the type of the argument that's
//! passed to the callback handler. Usually that should be the same as
//! \c obj_type, however this allows the implementation of handlers that
//! receives values that are derived from \c obj_type via a casting operator.
//!
//! By default, when a callback gets registered, the current value of the
//! Very ImPortant object gets reported by invoking the callback's
//! event() method, which also gets invoked when the Very ImPortant object's
//! value changes. The third parameter specifies a callback invocation
//! class (see \ref eventcallback "eventcallback") for post-registration
//! updates, and the fourth template parameter specifies a callback invocation
//! class for the initial value of the Very Important object, at registration
//! time. The default invocation class invokes the callback's event() method,
//! as described.
//!
//! The fifth parameter specifies the argument type that's passed to the
//! handlers, and defaults to the first template parameter. It may be used
//! to implement handlers that receive values that are derived by some
//! very important object's casting operator.
//!
//! The sixth template parameter specifies the type of the underlying
//! readers-writer mutex, and defaults to
//! \c INSERT_LIBX_NAMESPACE::rwmutex. It's not used directly, but, rather
//! gets sets to the debug rwmutex type by using
//! \ref vipobjdebug "INSERT_LIBX_NAMESPACE:vipobjdebug".

template<typename obj_type,
	 typename handler_type=eventcallbackbaseObj<obj_type>,
	 typename invoke_type=eventcallbackinvoke<obj_type, handler_type>,
	 typename initial_invoke_type=invoke_type,
	 typename arg_type=obj_type,
	 typename rwmutex_type=rwmutex>
class vipobj : public sipobj<obj_type, rwmutex_type> {

	//! Container for a  list of registered callbacks.
	typedef eventcallback<arg_type, handler_type, invoke_type> callbacks_t;

	//! A list of registered callbacks
	callbacks_t callbacks;

	//! A lock on updates to the callback handlers
	rwmutex_type updlock;

public:
	//! This template class instance type

	typedef vipobj<obj_type, handler_type, invoke_type,
		       initial_invoke_type, arg_type, rwmutex_type> vipobj_t;

	//! Constructor
	template<typename ...Args>
	vipobj(Args && ...args)
		: sipobj<obj_type, rwmutex_type>(std::forward<Args>(args)...)
	{
	}

	//! Destructor
	~vipobj() noexcept {}

	class updatelock;
	class handlerlock;

	friend class updatelock;
	friend class handlerlock;

	//! Helper template that creates a callback that invokes a lambda.

	//! The returned callback
	template<typename lambda_type>
	static ref<handler_type> create_callback(lambda_type &&lambda)
	{
		return callbacks_t
			::create_callback(std::forward<lambda_type>(lambda));
	}

	//! Update the value of the VIP object

	//! The \c vipobj_t that this lock gets constructed from must remain
	//! in scope as long as this lock object exists.
	//!
	//! Instanting this class blocks instantiation of
	//! \ref handlerlock "handlerlock"s and other
	//! \ref updatelock "updatelock"s.

	class updatelock {

		//! The underlying VIP object
		vipobj_t &objp;

		//! A mutex lock on updatemutex
		std::unique_lock<typename rwmutex_type::wmutex> lock;

	public:
		//! Invoke the callbacks with the updated value

		//! This is invoked by update(), after a
		//! \ref sipobj::writelock "writelock"-protected
		//! update completes.
		//!
		//! This may also be invoked manually, if a
		//! \ref sipobj::writelock "writelock" gets properly instantiated for
		//! the duration of the very important object update, and if it
		//! gets destroyed before invoking notify().
		//!
		//! If a \ref sipobj::writelock "writelock" gets destroyed without any
		//! changes to the very important object, it is not necessary
		//! to invoke this.

		void notify(const arg_type &obj)
		{
			objp.callbacks.event(obj);
		}

		//! The constructor takes a vipobj_t

		template<typename ...Args>
		updatelock(vipobj_t &objpArg, Args && ...args)
			: objp(objpArg), lock(objpArg.updlock.w,
					      std::forward<Args>(args)...)
		{
		}

		//! Destructor

		~updatelock() noexcept {}

		//! Update the very important object

		//! This updates the value of the very important object,
		//! while protected by an instantiated
		//! \ref sipobj::writelock "writelock", and then
		//! invokes notify().
		
		void update(const arg_type &obj)
		{
			{
				typename sipobj<obj_type, rwmutex_type>
					::writelock lock(objp);

				objp.obj=obj;
			}

			notify(obj);
		}
	};

	//! Register a new callback for changes to the VIP object

	//! The \c vipobj_t that this lock gets constructed from must remain
	//! in scope as long as this lock object exists.
	//!
	//! Instantiating this class blocks instantiation of
	//! \ref updatelock "updatelock"s.

	class handlerlock {

		//! The underlying VIP object
		vipobj_t &objp;

		//! A write lock on vipobj::handlerlock
		std::unique_lock<typename rwmutex_type::rmutex> lock;

	public:
		//! The constructor takes a vipobj_t
		template<typename ...Args>
		handlerlock(vipobj_t &objpArg, Args && ...args)
			: objp(objpArg), lock(objp.updlock.r,
					      std::forward<Args>(args)...)
		{
		}

		//! Destructor

		~handlerlock() noexcept {}

		//! Register a new callback

		//! The callback gets immediately invoked with the object
		//! value.

		void install(//! The handler being installed
			     const ref<handler_type> &h,

			     //! The alleged initial value of the very important object

			     const arg_type &v)
		{
			objp.callbacks.install(h);

			initial_invoke_type::event(h, v);
		}

		//! Register a new callback without immediately invoking it.

		//! The callback gets immediately invoked with the object
		//! value.

		void install(//! The handler being installed
			     const ref<handler_type> &h)
		{
			objp.callbacks.install(h);
		}
	};
};

#if 0
{
#endif
}
#endif

