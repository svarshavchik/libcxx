/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mpobj_H
#define x_mpobj_H

#include <x/mpthreadlockfwd.H>

#include <mutex>
#include <condition_variable>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Type of mutex protection

enum class mpobj_type {

		       //! Only a mutex
		       mutex_only,

		       //! A mutex with a condition variable
		       condvar,

		       //! A mutex with a thread-safe lock (and a condvar)
		       threadlock,
};

template<typename obj_type, typename mutex_type, mpobj_type type>
class mpobjimpl_lock;

template<typename obj_type, typename mutex_type, mpobj_type type>
class mpobjimpl;

//! Whether this mutex type has a condition variable

template <mpobj_type> struct has_condvar {

	static constexpr bool value=true;
};

//! mutex_only mutex does not have a condition variable

template <> struct has_condvar<mpobj_type::mutex_only> {

	static constexpr bool value=false;
};

//! Base class for a mutex without a condition variable.

template<bool> struct mpobj_condvar {};

//! Base class for a mutex with a condition variable.

template<> struct mpobj_condvar<true> {

	//! The condition variable
	std::condition_variable cond;

};

//! Whether this mutex type has a thread lock

template <mpobj_type> struct has_threadlock {

	static constexpr bool value=false;
};

//! Only threadlock type mpobjs have a thread lock.

template <> struct has_threadlock<mpobj_type::threadlock> {

	static constexpr bool value=true;
};

//! Base class for a mutex without a thread lock flag

template<bool> struct mpobj_threadlock {

	template<typename lock_type>
	static constexpr
	void wait_threadlock(lock_type &)
	{
	}

	template<typename mutex_type>
	static constexpr
	void try_threadlock(mutex_type &)
	{
	}
};

//! A mutex with an optional condition variable.

template<typename mutex_type, mpobj_type type>
struct mpobjmeta : mpobj_condvar<has_condvar<type>::value>,
	mpobj_threadlock<has_threadlock<type>::value> {

public:
	//! A mutex required to access the object
	mutex_type mutex;

	//! Constructor
	mpobjmeta()=default;

	//! Destructor
	~mpobjmeta()=default;
};

//! Methods of \ref mpobj "mpobj" and \ref mpcobj "mpcobj".

//! Instantiate a mutex, with an optional condition variable, define the
//! lock object, and all the methods.

template<typename obj_type, typename mutex_type, mpobj_type type>
class mpobjimpl {

	//! A mutex-protected object
	obj_type obj;

	//! Metadata for this mutex-protected object.
	mpobjmeta<mutex_type, type> meta;

public:

	//! This template class instance type

	typedef mpobjimpl<obj_type, mutex_type, type> mpobj_t;

	//! Constructor

	// Sadly, cannot use uniform initialization syntax here because
	// it warns about narrowing conversion when passing, say a literal
	// 100 constant to a constructor that takes a size_t parameter.

	template<typename ...Args_t>
	mpobjimpl(Args_t && ...args) : obj(std::forward<Args_t>(args)...) {}

	//! Destructor
	~mpobjimpl()=default;

	//! Alias
	typedef mpobjimpl_lock<obj_type, mutex_type, type> lock;

	//! Alias
	typedef mpthreadlock<obj_type, mutex_type> threadlock;

	friend lock;

	friend mpthreadlockObj<obj_type, mutex_type>;

	friend mpobj_threadlock<true>;

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

template<typename obj_type, typename mutex_type, mpobj_type type>
class mpobjimpl_lock {

	//! The mutex-protected type.

	typedef mpobjimpl<obj_type, mutex_type, type> mpobj_t;

public:
	//! The underlying VIP object
	mpobj_t &objp;
private:
	//! Whether the mutex has been locked
	std::unique_lock<mutex_type> mlock;

public:
	friend mpobj_threadlock<true>;

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
		{
			mlock.lock();
			objp.meta.wait_threadlock(*this);
		}
		else
		{
			(void)mlock.try_lock();
			objp.meta.try_threadlock(mlock);
		}
	}

	//! Reacquire the lock from an existing thread lock.

	mpobjimpl_lock(const mpthreadlock<obj_type, mutex_type> &threadlock)
		: objp{threadlock->objp},
		  mlock{objp.meta.mutex}
	{
		// Release the thread lock.
		threadlock->release();

		// Now, check to see if there are any other threadlock, by
		// any chance. If so, we'll wait for them to go away, too.
		objp.meta.wait_threadlock(*this);
	}

	//! Transfer the lock to another thread

	//! This returns a thread-safe handle for a reference-counted
	//! object. This handle can be passed to the constructor in a
	//! different execution thread to transfer the ownership of the
	//! mutex to the other execution thread.
	//!
	//! The mcguffin parameter is an opaque object that owns the
	//! underlying mutex-protected object. The returned mpthreadlock
	//! owns a reference on the mcguffin.
	//!
	//! This is implemented only in \ref mptobj "mptobj".

	mpthreadlock<obj_type, mutex_type> threadlock(const ref<obj> &mcguffin)
		const
	{
		return mpthreadlock<obj_type, mutex_type>::create(*this,
								  mcguffin);
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
	void wait(Predicate &&pred)
	{
		objp.meta.cond.wait(mlock, std::forward<Predicate>(pred));
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
	bool wait_for(const timeout_type &t, Predicate &&pred)
	{
		return objp.meta.cond.wait_for(mlock, t,
					       std::forward<Predicate>(pred));
	}

	//! Timed waiting on the condition variable, if this is an \ref mpcobj "mpcobj".

	template<typename timeout_type, class Predicate>
	auto wait_until(const timeout_type &t, Predicate &&pred)
		-> decltype(pred())
	{
		return objp.meta.cond.wait_until(mlock, t,
						 std::forward<Predicate>(pred));
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
using mpobj=mpobjimpl<obj_type, mutex_type, mpobj_type::mutex_only>;

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
//! Use \ref mpobj "mpobj" if you don't need a condition variable.
//!
//! Use \ref mptobj "mptobj" if you need a thread-safe lock.
//!
//! See also: \ref vipobj "Very Important object".

template<typename obj_type, typename mutex_type=std::mutex>
using mpcobj=mpobjimpl<obj_type, mutex_type, mpobj_type::condvar>;

//! A transferrable mutex-protected object, with a condition variable.

//! Like \ref mpcobj "mpcobj" but the mutex can be transferred to
//! another execution thread.
//!
//! mptobj implements a threadlock() method that returns an opaque handle
//! that carries the mutex lock to another execution thread. All locks
//! should not be constructed in dynamic scope, only in automatic scope,
//! since locks must be released by the same execution thread that acquired
//! them. An mptobj makes it possible to keep the underlying object locked
//! until a lock gets reacquired in a different execution thread.
//!
//! \code
//! typedef INSERT_LIBX_NAMESPACE::mptobj<some_t> mpsome_t;
//!
//! typedef INSERT_LIBX_NAMESPACE::mptobj_lock<some_t> mpsome_lock_t;
//!
//! class myStuffObj : virtual public INSERT_LIBX_NAMESPACE::obj {
//!
//! // ...
//!
//!     mpsome_t something;
//! };
//!
//! auto stuff=INSERT_LIBX_NAMESPACE::ref<myStuffObj>::create();
//!
//! // Execution thread 1:
//!
//! mpsome_lock_t lock{stuff->something};
//!
//! mpthreadlock<some_t> handle=lock.threadlock(stuff);
//!
//! // Execution thread 2:
//!
//! mpsome_lock_t lock{handle};
//! \endcode
//!
//! threadlock() returns a handle for a reference-counted object that holds
//! a logical lock on the underlying object, so the object remains logically
//! locked even after the real lock object goes out of scope
//! and gets destroyed.
//!
//! Constructing a lock object from the handle effectively transfer the
//! lock to a new lock object, in a different execution thread.
//!
//! This logical transfer occurs at most once. Using the same handle to
//! construct another lock object proceeds normally.
//!
//! This uses this mutex protected object's condition variable, to sequence
//! this process.
//!
//! The new lock object does not finish constructing until any existing
//! existing lock object, in the original execution thread, goes out of
//! scope and gets destroyed.
//!
//! Destroying the threadlock handle before instantiating a new real lock
//! object releases the logical lock.
//!
//! threadlock()'s sole parameter is an opaque object that the returned
//! thread lock handle owns a reference to.

template<typename obj_type, typename mutex_type=std::mutex>
using mptobj=mpobjimpl<obj_type, mutex_type, mpobj_type::threadlock>;

//! A lock on a \ref mpobj "mutex-protected object".

template<typename obj_type, typename mutex_type=std::mutex>
using mpobj_lock=mpobjimpl_lock<obj_type, mutex_type, mpobj_type::mutex_only>;

//! A lock on a \ref mpcobj "mutex-protected object with a condition variable".

template<typename obj_type, typename mutex_type=std::mutex>
using mpcobj_lock=mpobjimpl_lock<obj_type, mutex_type, mpobj_type::condvar>;

//! A lock on a \ref mptobj "transferable mutex-protected object".

template<typename obj_type, typename mutex_type=std::mutex>
using mptobj_lock=mpobjimpl_lock<obj_type, mutex_type, mpobj_type::threadlock>;

//! Implement transferrable mutex semantics.

template<> struct mpobj_threadlock<true> {

	//! Locked by a thread
	mpcobj<size_t> thread_lock_counter{0};

	//! Wait until no thread locks exist.

	template<typename lock_type>
	inline void wait_threadlock(lock_type &l)
	{
		while (1)
		{
			// The mutex-protected object is currently locked.

			// Now lock the thread counter, and make sure it's 0,
			// if so we're done.

			{
				mpcobj<size_t>::lock lock{thread_lock_counter};

				if (*lock == 0)
					return;

				// Unlock the main mutex, so that a different
				// thread can reacquire the main mutex in
				// order to adopt the transferred lock.
				//
				// Another thread lock can exist, but not yet
				// used to reacquire the main mutex, so we
				// must unlock it here, while waiting for
				// the other threadlock to go away...

				l.mlock.unlock();

				lock.wait([&]{ return *lock == 0; });
			}

			// .. and then reacquire the main mutex lock, we'll
			// loop back and check again.
			l.mlock.lock();
		}
	}

	//! If a thread lock exist, unlock myself

	template<typename mutex_type>
	inline void try_threadlock(mutex_type &m)
	{
		// If the main mutex didn't get locked, we bail out.

		if (!m.owns_lock())
			return;

		// If there's a thread lock, and we locked it, we admit
		// failure by unlocking ourselves.
		mpcobj<size_t>::lock lock{thread_lock_counter};

		if (*lock)
			m.unlock();
	}
};

#if 0
{
#endif
}
#endif
