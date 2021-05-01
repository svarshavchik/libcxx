/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sipobj_H
#define x_sipobj_H

#include <x/namespace.h>
#include <x/errors.H>

#include <shared_mutex>
#include <mutex>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A Somewhat-Important object

//! This template class takes some object, and exports access to the object
//! using a readers/writer locking design pattern.
//!
//! See also: \ref vipobj "Very Important object".

template<typename obj_type, typename mutex_type=std::shared_mutex>
class sipobj {

protected:

	//! The somewhat important object
	obj_type obj;

	//! A mutex for the somewhat important object
	mutex_type objlock;

public:

	//! This template class instance type

	typedef sipobj<obj_type, mutex_type> sipobj_t;

	//! Constructor
	template<typename ...Args>
	sipobj(Args && ...args) : obj(std::forward<Args>(args)...) {}

	//! Destructor
	~sipobj() {}

	class readlock;
	class writelock;

	friend class readlock;
	friend class writelock;

	//! Access the somewhat important object's current value

	//! The \c sipobj_t that this lock gets constructed from must remain
	//! in scope as long as this lock object exists.
	//!
	//! Instantiating this class blocks instantiation of
	//! \ref writelock "writelock"s.

	class readlock {

		//! The underlying VIP object
		sipobj_t &objp;

		//! An read lock on sipobj::objlock
		std::shared_lock<mutex_type> lock;

	public:
		//! The constructor takes a sipobj_t
		readlock(sipobj_t &objpArg) : objp(objpArg),
					      lock(objp.objlock)
		{
		}

		//! Try to acquire the lock.

		readlock(sipobj_t &objpArg, std::try_to_lock_t arg)
			: objp(objpArg),
			  lock(objp.objlock, arg)
		{
		}

		//! Try to acquire the lock.

		template <class Clock, class Duration>
		readlock(sipobj_t &objpArg,
			 const std::chrono::time_point<Clock, Duration> &t)
			: objp(objpArg), lock(objp.objlock, t)
		{
		}

		//! Try to acquire the lock.

		template <class Rep, class Period>
		readlock(sipobj_t &objpArg,
			 const std::chrono::duration<Rep, Period> &t)
			: objp(objpArg), lock(objp.objlock, t)
		{
		}

		//! Destructor
		~readlock() {}

		//! Whether the object was locked
		bool locked() const noexcept
		{
			return lock.owns_lock();
		}

		//! Access the underlying object
		const obj_type &operator*() const noexcept
		{
			if (!locked())
				null_ptr_deref();

			return objp.obj;
		}

		//! Access the underlying object
		const obj_type *operator->() const noexcept
		{
			if (!locked())
				null_ptr_deref();

			return &objp.obj;
		}
	};

	//! Update the somewhat important object

	//! The \c sipobj_t that this lock gets constructed from must remain
	//! in scope as long as this lock object exists.
	//!
	//! This lock allows the underlying object to be modified.
	//!
	//! Instantiating this class blocks instantiation of
	//! \ref readlock "readlock"s, and other
	//! \ref writelock "writelock"s.

	class writelock {

		//! The underlying VIP object
		sipobj_t &objp;

		//! A write lock on sipobj::objlock
		std::unique_lock<mutex_type> lock;

	public:
		//! The constructor takes a sipobj_t

		writelock(sipobj_t &objpArg) : objp(objpArg),
					       lock(objp.objlock)
		{
		}

		//! Try to acquire the lock.

		writelock(sipobj_t &objpArg, std::try_to_lock_t arg)
			: objp(objpArg),
			  lock(objp.objlock, arg)
		{
		}

		//! Try to acquire the lock.

		template <class Clock, class Duration>
		writelock(sipobj_t &objpArg,
			 const std::chrono::time_point<Clock, Duration> &t)
			: objp(objpArg), lock(objp.objlock, t)
		{
		}

		//! Try to acquire the lock.

		template <class Rep, class Period>
		writelock(sipobj_t &objpArg,
			 const std::chrono::duration<Rep, Period> &t)
			: objp(objpArg), lock(objp.objlock, t)
		{
		}

		//! Whether the object was locked
		bool locked() const noexcept
		{
			return lock.owns_lock();
		}

		//! Destructor
		~writelock() {}

		//! Access the underlying object
		obj_type &operator*() const noexcept
		{
			if (!locked())
				null_ptr_deref();

			return objp.obj;
		}

		//! Access the underlying object
		obj_type *operator->() const noexcept
		{
			if (!locked())
				null_ptr_deref();

			return &objp.obj;
		}
	};
};

#if 0
{
#endif
}
#endif
