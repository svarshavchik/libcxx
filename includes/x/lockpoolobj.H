/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_lockpoolobj_H
#define x_lockpoolobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/weakinfo.H>
#include <x/weakptr.H>
#include <x/weaklist.H>
#include <x/exceptionfwd.H>
#include <x/eventfd.H>

#include <list>
#include <set>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Implement default lock pool insert/delete operation

//! \internal
//! This template defines the default insert_lock() and delete_lock()
//! implementation for a lock pool.

template<typename lockid_t, typename lockpool_type>
class lockpool_traits {

public:
	//! Define the lock pool type

	typedef lockpool_type container_t;

	//! Default insert_lock() function

	//! The lock is inserted into the lock set
	//! \return true if the lock was inserted, false if the lock set
	//! already contains this lock

	static bool insert_lock(const lockid_t &v,
				container_t &l_set) noexcept
	{
		return l_set.insert(v).second;
	}

	//! Default delete_lock() function

	//! The lock is removed from the lock set

	static void delete_lock(const lockid_t &v,
				container_t &l_set) noexcept
	{
		l_set.erase(v);
	}
};

//! A lock pool object

//! \see lockpool.
//!

template<typename lockid_t, typename compare_t, typename lockop_t, bool starve>
class lockpoolObj : virtual public obj {

	//! Define myself

	typedef lockpoolObj<lockid_t, compare_t, lockop_t, starve> my_t;

	friend class lockentryObj;

	class placeholderObj;
public:
	//! Default constructor

	lockpoolObj();

	//! Default destructor
	~lockpoolObj();

	//! addLockSet() returns a reference to this object

	//! This object represents a set of locks that are either acquired
	//! or are waiting to be acquired.

	class lockentryObj : virtual public obj {

		//! Flag -- these locks have been acquired

		bool active;

		//! An event that gets notified when this lock set gets acquired.
		eventfd notifyevent;

		//! The lock pool to which this entry belongs

		ptr<my_t> mylockpool;

		//! Which locks this entry represents

		lockid_t lockvalue;

		//! Placeholder on the waiting list

		ptr<placeholderObj> placeholder;

	public:
		friend class lockpoolObj<lockid_t, compare_t, lockop_t, starve>;

		friend class placeholderObj;

		//! Default constructor

		lockentryObj(//! Locks to acquire.
			     const lockid_t &lockvalueArg,
			     //! Acquisition event file descriptor.

			     //! When the locks are acquired (immediately,
			     //! or when all other lock entries that block
			     //! this lock entry go out of scope), an event
			     //! gets reported to this file descriptor.
			     const eventfd &notifyeventArg,

			     //! My lock pool
			     const ptr<my_t> &mylockpoolArg)
			: active(false),
			  notifyevent(notifyeventArg),
			  mylockpool(mylockpoolArg),
			  lockvalue(lockvalueArg),
			  placeholder(ptr<placeholderObj>::create())
		{
		}

		//! Default destructor

		~lockentryObj();

		//! Indicate if this lock has been acquired

		//! \return \c true if this lock is currently acquired.
		//! If \c false, try again later, or wait for the event
		//! to get reported.

		bool locked()
		{
			std::lock_guard<std::mutex> lock(objmutex);

			return active;
		}

		//! Retrieve the event object where lock acquisition gets reported to

		//! \return \c the event file descriptor that was passed to the
		//! constructor.

		eventfd getNotifyEvent() noexcept
		{
			std::lock_guard<std::mutex> lock(objmutex);

			return notifyevent;
		}

	};

	//! Return value from addLockSet().

	typedef ref<lockentryObj> lockentry;

	//! A nullable pointer to what addLockSet() returns

	typedef ptr<lockentryObj> lockentryptr;

private:

	//! All locks that have been acquired

	//! Access to this object is protected by objmutex.
	//!
	class lockop_t::container_t active_locks;

	//! Waiting locks

	//! These locks have not been acquired yet. This is a list of
	//! weak references, so our reference to each object does not count.
	//! If it gets destroyed because of its other, strong, references
	//! going out of scope, it gets removed from this list.

	weaklist<placeholderObj> waiting_list;

	//! Placeholder for locks that have not been acquired yet.

	//! This placeholder gets placed on the
	//! \ref waiting_list "waiting_list", and held in the lock object.
	//! Once the lock gets acquired, the placeholder object gets removed
	//! from the lock object, which removes it from the waiting list.
	//! If the lock object goes out of scope, the placeholder also goes
	//! away, and gets removed from the waiting list.

	class placeholderObj : virtual public obj {

	public:
		//! Constructor
		placeholderObj() {}

		//! Destructor
		~placeholderObj() {}

		//! Which lock entry this is a placeholder for.
		weakptr< ptr<lockentryObj> > lockentryref;
	};

public:

	//! Add a lock entry to this lock pool.

	//! \return a reference to a lockentry object representing the given
	//! locked value.
	//! An attempt gets made to acquire the specified locks immediately.
	//! If so, the returned lockentry object already represents a locked
	//! object, and the acquisition event gets reported.
	//!
	//! When the caller no longer wants to hold this lock, the returned
	//! object must go out of scope. The locks are released only when
	//! the underlying reference-counted object gets destroyed after all
	//! of its references are no longer in scope.

	lockentry addLockSet(//! Locks to acquire.
			     const lockid_t &lockvalue,
			     //! Acquisition event file descriptor.

			     //! When the locks are acquired (immediately,
			     //! or when all other lock entries that block
			     //! this lock entry go out of scope), an event
			     //! gets reported to this file descriptor.
			     const eventfd &event=eventfd())
	{
		lockentry newEntry=lockentry::create(lockvalue, event,
						     ptr<my_t>(this));

		newEntry->placeholder->lockentryref=newEntry;

		// Everything goes to a waiting list, first.
		waiting_list->push_back(newEntry->placeholder);

		checklocks();

		return newEntry;
	}


private:
	//! Attempt to acquire locks

	//! Go through the list of waiting lock entries, and check if they
	//! can now be acquired.

	void checklocks() noexcept
	{
		// Weaklist exports thread-safe iterators
		for (auto wplaceholder: *waiting_list)
		{
			ptr<placeholderObj> placeholder=wplaceholder.getptr();

			if (placeholder.null())
				continue;

			ptr<lockentryObj> candidate=
				placeholder->lockentryref.getptr();

			placeholder=ptr<placeholderObj>();

			if (candidate.null())
				continue;

			lockentryObj &nextEntryRef= *candidate;

			std::lock_guard<std::mutex>
				lock(nextEntryRef.objmutex);

			if (nextEntryRef.active)
				continue; // Another thread grabbed this one

			bool flag;

			{
				std::lock_guard<std::mutex> lock(objmutex);

				flag=lockop_t
					::insert_lock(nextEntryRef.lockvalue,
						      active_locks);
			}

			if (!flag)
			{
				if (!starve)
					return;

				// Couldn't acquire the lock, keep going.
				continue;
			}

			nextEntryRef.active=true;
			nextEntryRef.placeholder=placeholder;
			nextEntryRef.notifyevent->event(1);
		}
	}
};

template<typename lockid_t, typename compare_t, typename lockop_t, bool starve>
	lockpoolObj<lockid_t, compare_t, lockop_t, starve>::lockpoolObj()
		: waiting_list(weaklist<placeholderObj>::create())
{
}

template<typename lockid_t, typename compare_t, typename lockop_t, bool starve>
lockpoolObj<lockid_t, compare_t, lockop_t, starve>::~lockpoolObj()
{
}

/* BUG: gcc 4.6 barfs if this is declared inline in the class */

template<typename lockid_t, typename compare_t, typename lockop_t, bool starve>
lockpoolObj<lockid_t, compare_t, lockop_t, starve>::lockentryObj
::~lockentryObj()
{
#ifdef LIBCXX_LOCKPOOL_DEBUG
	LIBCXX_LOCKPOOL_DEBUG();
#endif
	placeholder=ptr<placeholderObj>();

	if (!active)
		return;

	{
		std::lock_guard<std::mutex> lock(mylockpool->objmutex);
		lockop_t::delete_lock(lockvalue,
				      mylockpool->active_locks);
	}

	mylockpool->checklocks();
}

#if 0
{
#endif
}
#endif

