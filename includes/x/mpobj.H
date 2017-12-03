/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mpobj_H
#define x_mpobj_H

#include <x/namespace.h>

#include <mutex>
#include <condition_variable>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename mutex_type, bool> class mpobjmeta;

//! A mutex and a condition variable of an \ref mpcobj "mpcobj".

template<typename mutex_type>
class mpobjmeta<mutex_type, true> {
public:
	//! A mutex required to access the object
	mutex_type mutex;

	//! The condition variable
	std::condition_variable cond;

	//! Constructor
	mpobjmeta() {}

	//! Destructor
	~mpobjmeta() {}
};

//! Just a mutex of an \ref mpobj "mpobj".

template<typename mutex_type>
class mpobjmeta<mutex_type, false> {
public:
	//! A mutex required to access the object
	mutex_type mutex;

	//! Constructor
	mpobjmeta() {}

	//! Destructor
	~mpobjmeta() {}
};

template<typename obj_type, typename mutex_type, bool hascondvar>
class mpobjimpl_lock;

//! Methods of \ref mpobj "mpobj" and \ref mpcobj "mpcobj".

//! Instantiate a mutex, with an optional condition variable, define the
//! lock object, and all the methods.

template<typename obj_type, typename mutex_type, bool hascondvar>
class mpobjimpl {

	//! A mutex-protected object
	obj_type obj;

	//! Metadata for this mutex-protected object.
	mpobjmeta<mutex_type, hascondvar> meta;

public:

	//! This template class instance type

	typedef mpobjimpl<obj_type, mutex_type, hascondvar> mpobj_t;

	//! Constructor

	// Sadly, cannot use uniform initialization syntax here because
	// it warns about narrowing conversion when passing, say a literal
	// 100 constant to a constructor that takes a size_t parameter.

	template<typename ...Args_t>
	mpobjimpl(Args_t && ...args) : obj(std::forward<Args_t>(args)...) {}

	//! Destructor
	~mpobjimpl() {}

	//! Alias
	typedef mpobjimpl_lock<obj_type, mutex_type, hascondvar> lock;

	friend lock;

	//! The assignment operator

	//! A shortcut for acquiring a lock, using it to set the
	//! value, then releasing the lock.

	template<typename V>
	auto &operator=(V &&v)
	{
		lock l{*this};

		*l=std::forward<V>(v);
		return *this;
	}

	//! get() the value.

	//! A shortcut for acquiring a lock, using it to get the
	//! mutex-protected value, then releasing the lock.

	auto get()
	{
		lock l{*this};

		return *l;
	}
};

//! A lock on a mutex-protected object.

template<typename obj_type, typename mutex_type, bool hascondvar>
class mpobjimpl_lock {

	//! The mutex-protected type.

	typedef mpobjimpl<obj_type, mutex_type, hascondvar> mpobj_t;

	//! The underlying VIP object
	mpobj_t &objp;

	//! Whether the mutex has been locked
	std::unique_lock<mutex_type> mlock;

public:
	//! The constructor takes a mpobj_t
	mpobjimpl_lock(//! The mutex-protected object to lock
		       mpobj_t &objpArg,

		       //! Whether to wait until a lock is available, or return
		       //! immediately without locking it.
		       //! Use locked() to see if this lock was
		       //! acquired.

		       bool waitflag=true)
		: objp(objpArg), mlock(objp.meta.mutex,
				       std::defer_lock_t())
	{
		if (waitflag)
			mlock.lock();
		else
			mlock.try_lock();
	}

	//! Whether the lock was acquired

	bool locked() const { return mlock.owns_lock(); }

	//! Destructor
	~mpobjimpl_lock()=default;

	//! Access the underlying object
	const obj_type &operator*() const noexcept
	{
		return objp.obj;
	}

	//! Access the underlying object
	const obj_type *operator->() const noexcept
	{
		return &objp.obj;
	}

	//! Access the underlying object
	obj_type &operator*() noexcept
	{
		return objp.obj;
	}

	//! Access the underlying object
	obj_type *operator->() noexcept
	{
		return &objp.obj;
	}

	//! Signal the condition variable, if this is an \ref mpcobj "mpcobj".
	void notify_one()
	{
		objp.meta.cond.notify_one();
	}

	//! Broadcast the condition variable, if this is an \ref mpcobj "mpcobj"/
	void notify_all()
	{
		objp.meta.cond.notify_all();
	}

	//! Wait on the condition variable, if this is an \ref mpcobj "mpcobj".

	void wait()
	{
		objp.meta.cond.wait(mlock);
	}

	//! Wait on the condition variable, if this is an \ref mpcobj "mpcobj".
	template<class Predicate>
	void wait(Predicate pred)
	{
		objp.meta.cond.wait(mlock, pred);
	}

	//! Timed waiting on the condition variable, if this is an \ref mpcobj "mpcobj".

	template<typename timeout_type>
	bool wait_for(const timeout_type &t)
	{
		return objp.meta.cond.wait_for(mlock, t) == std::cv_status::no_timeout;
	}

	//! Timed waiting on the condition variable, if this is an \ref mpcobj "mpcobj".

	template<typename timeout_type>
	bool wait_until(const timeout_type &t)
	{
		return objp.meta.cond.wait_until(mlock, t) == std::cv_status::no_timeout;
	}

	//! Timed waiting on the condition variable, if this is an \ref mpcobj "mpcobj".

	template<typename timeout_type, class Predicate>
	bool wait_for(const timeout_type &t, Predicate pred)
	{
		return objp.meta.cond.wait_for(mlock, t, pred);
	}

	//! Timed waiting on the condition variable, if this is an \ref mpcobj "mpcobj".

	template<typename timeout_type, class Predicate>
	auto wait_until(const timeout_type &t, Predicate pred)
		-> decltype(pred())
	{
		return objp.meta.cond.wait_until(mlock, t, pred);
	}

};

//! A mutex-protected object

//! This template class wraps the template parameter object with a mutex.
//! Accessing the object requires acquiring a mutex lock.
//!
//! The optional second parameter defaults to
//! \c std::mutex, and can be overriden, if you know what you're doing.
//!
//! See \ref mpobjimpl "mpobjimpl" for method descriptions.
//!
//! Use \ref mpcobj "mpcobj" if you need a condition variable to go with the
//! mutex.
//!
//! See also: \ref vipobj "Very Important object".

template<typename obj_type, typename mutex_type=std::mutex>
using mpobj=mpobjimpl<obj_type, mutex_type, false>;

//! A mutex-protected object, with a condition variable.

//! This template class wraps the template parameter object with a mutex
//! and a condition variable. Accessing the object requires acquiring a mutex
//! lock.
//!
//! The optional second parameter defaults to
//! \c std::mutex, and can be overriden, if you know what you're doing.
//!
//! See \ref mpobjimpl "mpobjimpl" for method descriptions.
//!
//! Use \ref mpcobj "mpcobj" if you need a condition variable to go with the
//! mutex.
//!
//! See also: \ref vipobj "Very Important object".

template<typename obj_type, typename mutex_type=std::mutex>
using mpcobj=mpobjimpl<obj_type, mutex_type, true>;

//! A lock on a \ref mpobj "mutex-protected object".

template<typename obj_type, typename mutex_type=std::mutex>
using mpobj_lock=mpobjimpl_lock<obj_type, mutex_type, false>;

//! A lock on a \ref mpcobj "mutex-protected object with a condition variable".

template<typename obj_type, typename mutex_type=std::mutex>
using mpcobj_lock=mpobjimpl_lock<obj_type, mutex_type, true>;

#if 0
{
#endif
}
#endif
