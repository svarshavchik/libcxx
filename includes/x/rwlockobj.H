/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_rwlockobj_H
#define x_rwlockobj_H

#include <time.h>
#include <x/ref.H>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/rwmutex.H>
#include <x/rwlockfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A reference-counted object with rwmutex semantics.

//! This makes it possible to use reference-counted
//! rwmutexes.

class rwlockObj : virtual public obj {

	//! The underlying mutex.

	rwmutex rw;

public:

	//! Create a new rwlock
	rwlockObj();

	//! Default destructor
	~rwlockObj() noexcept;

	//! A reference-counted read lock

	class rlockObj : virtual public obj {

		//! The locked read/write lock.
		rwlockptr lock;

		//! The read lock;

		std::unique_lock<rwmutex::rmutex> rlock;

		//! Private constructor
		rlockObj(const rwlock &lockArg);

	public:
		friend class rwlockObj;
		friend class ptrrefBase;

		rlockObj();
		~rlockObj() noexcept;
	};

	//! A reference-counted write lock

	class wlockObj : virtual public obj {

		//! The locked read/write lock.
		rwlockptr lock;

		//! The write lock

		std::unique_lock<rwmutex::wmutex> wlock;

		//! Private constructor
		wlockObj(const rwlock &lockArg);

	public:
		friend class rwlockObj;
		friend class ptrrefBase;

		wlockObj();
		~wlockObj() noexcept;
	};

	friend class rlockObj;
	friend class wlockObj;

	//! A reference to a read lock

	typedef ref<rlockObj> rlock;

	//! A nullable reference to a read lock

	typedef ptr<rlockObj> rlockptr;

	//! A reference to a write lock

	typedef ref<wlockObj> wlock;

	//! A nullable reference to a write lock

	typedef ptr<wlockObj> wlockptr;

	//! Acquire a lock, however long it takes.

	rlock readlock();

	//! Try a lock

	//! If the lock was acquired, return an rlock, otherwise a null.

	rlockptr try_readlock();

	//! Try a lock

	//! If the lock was acquired, return an rlock, otherwise a null.

	template<typename timeout_type>
	rlockptr try_readlock_until(const timeout_type &abstime)
	{
		rlockptr ptr;

		rlock r=rlock::create(rwlock(this));

		r->lock=rwlock(this);
		if (r->rlock.try_lock_until(abstime))
			ptr=r;

		return ptr;
	}

	//! Try a lock

	//! If the lock was acquired, return an rlock, otherwise a null.

	template<typename timeout_type>
	rlockptr try_readlock_for(const timeout_type &reltime)
	{
		rlockptr ptr;

		rlock r=rlock::create(rwlock(this));

		if (r->rlock.try_lock_for(reltime))
			ptr=r;

		return ptr;
	}

	//! Acquire a lock, however long it takes.

	wlock writelock();

	//! Try a lock

	//! If the lock was acquired, return an wlock, otherwise a null.

	wlockptr try_writelock();

	//! Try a lock

	//! If the lock was acquired, return an wlock, otherwise a null.

	template<typename timeout_type>
	wlockptr try_writelock_until(const timeout_type &abstime)
	{
		wlockptr ptr;

		wlock w=wlock::create(rwlock(this));

		if (w->wlock.try_lock_until(abstime))
			ptr=w;

		return ptr;
	}

	//! Try a lock

	//! If the lock was acquired, return an wlock, otherwise a null.

	template<typename timeout_type>
	wlockptr try_writelock_for(const timeout_type &reltime)
	{
		wlockptr ptr;

		wlock w=wlock::create(rwlock(this));

		if (w->wlock.try_lock_for(reltime))
			ptr=w;

		return ptr;
	}

};

#if 0
{
#endif
}
#endif

