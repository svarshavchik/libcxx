/*
** Copyright 2012-2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sharedlockobj_H
#define x_sharedlockobj_H

#include <time.h>
#include <x/ref.H>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/sharedlockfwd.H>
#include <x/namespace.h>

#include <shared_mutex>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A reference-counted object with std::shared_mutex semantics.

//! This makes it possible to use reference-counted std::shared_mutexes.

class sharedlockObj : virtual public obj {

	//! The underlying mutex.

	std::shared_timed_mutex rw;

public:

	//! Create a new sharedlock
	sharedlockObj();

	//! Default destructor
	~sharedlockObj();

	//! A reference-counted read lock

	class sharedObj : virtual public obj {

		//! The locked read/write lock.
		sharedlockptr lock;

		//! The read lock;

		std::shared_lock<std::shared_timed_mutex> shared;

		//! Private constructor
		sharedObj(const sharedlock &lockArg);

	public:
		friend class sharedlockObj;
		friend class ptrrefBase;

		sharedObj();
		~sharedObj();
	};

	//! A reference-counted write lock

	class uniqueObj : virtual public obj {

		//! The locked read/write lock.
		sharedlockptr lock;

		//! The write lock

		std::unique_lock<std::shared_timed_mutex> unique;

		//! Private constructor
		uniqueObj(const sharedlock &lockArg);

	public:
		friend class sharedlockObj;
		friend class ptrrefBase;

		uniqueObj();
		~uniqueObj();
	};

	friend class sharedObj;
	friend class uniqueObj;

	//! A reference to a read lock

	typedef ref<sharedObj> shared;

	//! A nullable reference to a read lock

	typedef ptr<sharedObj> sharedptr;

	//! A reference to a write lock

	typedef ref<uniqueObj> unique;

	//! A nullable reference to a write lock

	typedef ptr<uniqueObj> uniqueptr;

	//! Acquire a lock, however long it takes.

	shared create_shared();

	//! Try a lock

	//! If the lock was acquired, return an shared, otherwise a null.

	sharedptr try_shared();

	//! Try a lock

	//! If the lock was acquired, return an shared, otherwise a null.

	template<typename timeout_type>
	sharedptr try_shared_until(const timeout_type &abstime)
	{
		sharedptr ptr;

		shared r=shared::create(sharedlock(this));

		r->lock=sharedlock(this);
		if (r->shared.try_lock_until(abstime))
			ptr=r;

		return ptr;
	}

	//! Try a lock

	//! If the lock was acquired, return an shared, otherwise a null.

	template<typename timeout_type>
	sharedptr try_shared_for(const timeout_type &reltime)
	{
		sharedptr ptr;

		shared r=shared::create(sharedlock(this));

		if (r->shared.try_lock_for(reltime))
			ptr=r;

		return ptr;
	}

	//! Acquire a lock, however long it takes.

	unique create_unique();

	//! Try a lock

	//! If the lock was acquired, return an unique, otherwise a null.

	uniqueptr try_unique();

	//! Try a lock

	//! If the lock was acquired, return an unique, otherwise a null.

	template<typename timeout_type>
	uniqueptr try_unique_until(const timeout_type &abstime)
	{
		uniqueptr ptr;

		unique w=unique::create(sharedlock(this));

		if (w->unique.try_lock_until(abstime))
			ptr=w;

		return ptr;
	}

	//! Try a lock

	//! If the lock was acquired, return an unique, otherwise a null.

	template<typename timeout_type>
	uniqueptr try_unique_for(const timeout_type &reltime)
	{
		uniqueptr ptr;

		unique w=unique::create(sharedlock(this));

		if (w->unique.try_lock_for(reltime))
			ptr=w;

		return ptr;
	}

};

#if 0
{
#endif
}
#endif
