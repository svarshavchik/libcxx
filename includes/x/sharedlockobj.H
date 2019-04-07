/*
** Copyright 2012-2019 Double Precision, Inc.
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

#include <x/mpobj.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A reference-counted object with std::shared_mutex semantics.

//! This makes it possible to use reference-counted std::shared_mutexes.

class sharedlockObj : virtual public obj {

	//! Counters of shared and exclusive locks in existence.

	struct lock_info_t {
		size_t shared_lockers=0;
		bool exclusively_locked=false;

		inline bool ok_to_lock_shared() const
		{
			return !exclusively_locked;
		}

		inline bool ok_to_lock_exclusively() const
		{
			return !exclusively_locked && shared_lockers == 0;
		}
	};

	//! Protected information.

	mpcobj<lock_info_t> lock_info;

public:

	//! Create a new sharedlock
	sharedlockObj();

	//! Default destructor
	~sharedlockObj();

	//! A reference-counted read lock

	class sharedObj : virtual public obj {

		//! The locked read/write lock.
		const sharedlock lock;
	public:

		sharedObj(const sharedlock &,
			  mpcobj<lock_info_t>::lock &lock);
		~sharedObj();
	};

	//! A reference-counted write lock

	class uniqueObj : virtual public obj {

		//! The locked read/write lock.
		const sharedlock lock;

	public:
		uniqueObj(const sharedlock &,
			  mpcobj<lock_info_t>::lock &lock);
		~uniqueObj();
	};

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
	sharedptr try_shared_until(timeout_type &&abstime)
	{
		sharedptr ptr;

		mpcobj<lock_info_t>::lock l{lock_info};

		if (l.wait_until(std::forward<timeout_type>(abstime),
				 [&]
				 {
					 return l->ok_to_lock_shared();
				 }))
		{
			ptr=shared::create(ref{this}, l);
		}
		return ptr;
	}

	//! Try a lock

	//! If the lock was acquired, return an shared, otherwise a null.

	template<typename timeout_type>
	sharedptr try_shared_for(timeout_type &&reltime)
	{
		sharedptr ptr;

		mpcobj<lock_info_t>::lock l{lock_info};

		if (l.wait_for(std::forward<timeout_type>(reltime),
				 [&]
				 {
					 return l->ok_to_lock_shared();
				 }))
		{
			ptr=shared::create(ref{this}, l);
		}
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
	uniqueptr try_unique_until(timeout_type &&abstime)
	{
		uniqueptr ptr;

		mpcobj<lock_info_t>::lock l{lock_info};

		if (l.wait_until(std::forward<timeout_type>(abstime),
				 [&]
				 {
					 return l->ok_to_lock_exclusively();
				 }))
		{
			ptr=unique::create(ref{this}, l);
		}
		return ptr;
	}

	//! Try a lock

	//! If the lock was acquired, return an unique, otherwise a null.

	template<typename timeout_type>
	uniqueptr try_unique_for(timeout_type &&reltime)
	{
		uniqueptr ptr;

		mpcobj<lock_info_t>::lock l{lock_info};

		if (l.wait_for(std::forward<timeout_type>(reltime),
			       [&]
			       {
				       return l->ok_to_lock_exclusively();
			       }))
		{
			ptr=unique::create(ref{this}, l);
		}
		return ptr;
	}
};

#if 0
{
#endif
}
#endif
