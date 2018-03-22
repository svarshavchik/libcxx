/*
** Copyright 2012-2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_vipobj_H
#define x_vipobj_H

#include <x/namespace.h>
#include <x/sipobj.H>
#include <x/weaklist.H>
#include <x/functionalrefptr.H>
#include <x/invoke_callbacks.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A Very Important object

//! Define a template for a Very ImPortant object: an object, and a list
//! of lambdas, with consistent semantics for notifying the
//! registered lambdas when the Very ImPortant object changes.
//!
//! The Very Important object is a further subclass of a
//! \ref sipobj "Somewhat Important Object", that supplements its
//! read-write lock access mechanism with two additional locks: a handler lock
//! and an update lock, that implements a callback-type facility.
//!
//! When a new lambda gets registered, it gets (optionally) invoked and
//! given the current value of the Very ImPortant object, then subsequent
//! changes to the objects are reported to the callback, until the callback
//! goes out of scope.
//!
//! Registering a lambda returns a \ref functionref "callback objects", a
//! reference-counted object. The lambda gets invoked until the last reference
//! to the callback goes out of scope, and it gets destroyed. The callback
//! objects are held in an internal \ref weaklist "weak list".
//!
//! Careful locking is used to provide a consistent reporting of changes to
//! the value of the object in a multithreaded context. There's also a concept
//! of obtaining write access to the object, without reporting the write
//! access to the registered callbacks.
//!
//! The first template parameter specifies the Very ImPortant object class.
//! The second parameter specifies the type of the parameter passed to the
//! callback lambdas, which defaults to "const object_type &".
//! The optional 3rd template defaults to false. If it's true, any
//! \ref exception "exceptions" thrown by a callback are trapped, and logged.
//! The optional 4th template parameter specifies the type of the underlying
//! readers-writer mutex, and defaults to
//! \c std::shared_mutex.

template<typename obj_type,
	 typename callback_arg_type=const obj_type &,
	 bool trap_exceptions=false,
	 typename mutex_type=std::shared_mutex>
class vipobj : public sipobj<obj_type, mutex_type> {

	//! Container for a  list of registered callbacks.
	typedef weaklist<functionObj<void(callback_arg_type)>> callbacks_t;

	//! The type of the callback used internally.
	typedef functionref<void(callback_arg_type)> callback_t;

	//! A list of registered callbacks
	callbacks_t callbacks;

	//! A lock on updates to the callback handlers
	mutex_type updlock;

public:
	//! This template class instance type

	typedef vipobj<obj_type, callback_arg_type, trap_exceptions,
		       mutex_type> vipobj_t;

	//! The object in here
	typedef obj_type obj_t;

	//! The very important object parameter to handlers
	typedef callback_arg_type callback_arg_type_t;

	//! Constructor
	template<typename ...Args>
	vipobj(Args && ...args)
		: sipobj<obj_type, mutex_type>(std::forward<Args>(args)...),
		callbacks(callbacks_t::create())
	{
	}

	//! Destructor
	~vipobj() {}

	class updatelock;
	class handlerlock;

	friend class updatelock;
	friend class handlerlock;

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
		std::unique_lock<mutex_type> lock;

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
		template<typename notify_value_type>
		void notify(const notify_value_type &obj)
		{
			if (trap_exceptions)
			{
				invoke_callbacks_log_exceptions
					(objp.callbacks, obj);
			}
			else
				invoke_callbacks(objp.callbacks, obj);
		}

		//! The constructor takes a vipobj_t

		template<typename ...Args>
		updatelock(vipobj_t &objpArg, Args && ...args)
			: objp(objpArg), lock(objpArg.updlock,
					      std::forward<Args>(args)...)
		{
		}

		//! Destructor

		~updatelock() {}

		//! Update the very important object

		//! This updates the value of the very important object,
		//! while protected by an instantiated
		//! \ref sipobj::writelock "writelock", and then
		//! invokes notify().

		void update(const obj_type &obj)
		{
			{
				typename sipobj<obj_type, mutex_type>
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
		std::shared_lock<mutex_type> lock;

	public:
		//! The constructor takes a vipobj_t
		template<typename ...Args>
		handlerlock(vipobj_t &objpArg, Args && ...args)
			: objp(objpArg), lock(objp.updlock,
					      std::forward<Args>(args)...)
		{
		}

		//! Destructor

		~handlerlock() {}

		//! Register a new callback

		//! It's added to the beginning of the list of the existing
		//! callbacks.
		template<typename functor_type>
		auto install_front(//! The handler being installed
				   functor_type &&functor)
		{
			callback_t cb{std::forward<functor_type>(functor)};

			objp.callbacks->push_front(cb);
			return cb;
		}

		//! Register a new callback

		//! It's added to the beginning of the list of the existing
		//! callbacks.
		//! The callback gets immediately invoked with the current
		//! value of this very important object.

		template<typename functor_type, typename initial_value_type>
		auto install_front(//! The handler being installed
				   functor_type &&functor,

				   //! The alleged initial value of the very important object

				   const initial_value_type &v)
		{
			auto cb=install_front(std::forward<functor_type>
					      (functor));
			cb(v);

			return cb;
		}

		//! Register a new callback

		//! It's added to the end of the list of the existing
		//! callbacks.
		template<typename functor_type>
		auto install_back(//! The handler being installed
				  functor_type &&functor)
		{
			callback_t cb{std::forward<functor_type>(functor)};

			objp.callbacks->push_back(cb);

			return cb;
		}

		//! Register a new callback

		//! It's added to the end of the list of the existing
		//! callbacks.
		//! The callback gets immediately invoked with the current
		//! value of this very important object.

		template<typename functor_type, typename initial_value_type>
		auto install_back(//! The handler being installed
				  functor_type &&functor,

				  //! The alleged initial value of the very important object

				  const initial_value_type &v)
		{
			auto cb=install_back(std::forward<functor_type>
					     (functor));
			cb(v);

			return cb;
		}

		//! Attach an existing object as a callback

		//! The first parameter is some existing object. The
		//! second parameter is a lambda. The third parameter is the
		//! current value of the very important object.
		//! The lambda gets invoked with two parameters, the object,
		//! and the initial or the new value of the very important
		//! object; as long as the object itself continues to exist.

		template<typename class_type, typename base_type,
			 typename lambda_type,
			 typename ...Args>
		void attach_front(const ref<class_type, base_type> &obj,
				  lambda_type &&lambda,
				  Args ...args)
		{
			auto mcguffin=install_front
				([weakptr=weakptr<ptr<class_type, base_type>>
				  (obj),
				  lambda]
				 (callback_arg_type arg)
				 {
					 auto obj=weakptr.getptr();

					 if (obj.null())
						 return;

					 lambda(obj, arg);
				 }, std::forward<Args>(args)...);

			obj->ondestroy([mcguffin]
				       {
				       });
		}

		//! Attach an existing object as a callback

		//! The first parameter is some existing object. The
		//! second parameter is a lambda. The third parameter is the
		//! current value of the very important object.
		//! The lambda gets invoked with two parameters, the object,
		//! and the initial or the new value of the very important
		//! object; as long as the object itself continues to exist.

		template<typename class_type, typename base_type,
			 typename lambda_type,
			 typename ...Args>
		void attach_back(const ref<class_type, base_type> &obj,
				 lambda_type &&lambda,
				  Args ...args)
		{
			auto mcguffin=install_back
				([weakptr=weakptr<ptr<class_type, base_type>>
				  (obj),
				  lambda]
				 (callback_arg_type arg)
				 {
					 auto obj=weakptr.getptr();

					 if (obj.null())
						 return;

					 lambda(obj, arg);
				 }, std::forward<Args>(args)...);

			obj->ondestroy([mcguffin]
				       {
				       });
		}
	};
};

#if 0
{
#endif
}
#endif
