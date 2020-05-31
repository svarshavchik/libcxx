/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_lockpool_H
#define x_lockpool_H

#include <x/namespace.h>
#include <x/lockpoolobj.H>
#include <x/weaklist.H>
#include <x/weakptr.H>
#include <x/ref.H>
#include <set>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename lockid_t, typename compare_t, typename traits_t, bool starve>
class lockpoolBase;

//! A pool of locks that use event file descriptors

//! The lockpool template defines a reference-counted object that maintains
//! a pool of locks. Each lock in the pool carries a unique identifier.
//! An attempt to place a lock into the pool may block; usually if another
//! lock with the same identifier is already in the pool (the exact conditions
//! may be specified in the pool traits class). If so, the lock is placed on
//! a waiting list. When the blocking lock gets removed from the pool, the
//! waiting lock then gets placed into the pool automatically. An associated
//! event file descriptor get signalled when the lock is finally placed into
//! the pool.
//!
//! \code
//!
//! typedef lockpool<std::string> mylockpool_t;
//!
//! mylockpool_t mylockpool(mylockpool_t::create());
//! \endcode
//!
//! The above example defines a lock pool where individual locks are named as
//! ordinary strings. Only one unique lock may be held in a single
//! lock pool. More than one lock may be held, as long as they are unique.
//! If an attempt is made to place a lock in a lock pool that already holds
//! the same lock, the second lock gets placed in a waiting status, until the
//! existing lock with the same name is removed from the pool.
//!
//! The above example implements a pool of locks that are identified by
//! ordinary strings. The lockpool template takes three parameters:
//!
//! <div style="margin-left: 3em">
//! <ul>
//! <li>\c lockid_t - the object lock type, such as \c std::string. This
//! is the only required template parameter.</li>
//!
//! <li>\c compare_t - A comparison function for object lock types. Defaults to
//! \c std::less&lt;lockid_t&gt;.</li>
//!
//! <li>\c traits_t - The object pool traits class.</li>
//!
//! <li>\c bool starve - a boolean flag that selects whether older waiting
//! locks can be starved by newer waiting locks.</li>
//! </ul>
//! </div>
//!
//! \c lockid_t specifies the types of individual locks, and \c compare_t
//! is the comparison function. For a basic
//! implementation where locks are identified by strings, \c std::string
//! serves as a good choice.
//!
//! The third template parameter specifies the traits class. This traits
//! class must define four class members:
//!
//! <div style="margin-left: 3em">
//! <ul>
//! <li>\c container_t - the container for the acquired locks currently held
//! in the pool. The default traits class defines \c container_t as
//! \c std::set&lt;lockid_t, \c compare_t&gt;.</li>
//!
//! <li>\c bool insert_lock(\c const \c lockid_t &amp;, \c container_t &amp;) -
//! a static class method that adds a lock to the lock container, returning
//! \c true if the lock was added, and \c false if the container already
//! contains the lock. The container must remain unchanged if \c insert_lock()
//! returns \c false (although it's acceptable to modify the container
//! temporarily while in the process of adding the lock, and restoring it to
//! its original contents before returning).</li>
//!
//! <li>\c void delete_lock(\c const \c lockid_t &amp;, \c container_t &amp;) -
//! a static class method that removes an lock from the lock container, this
//! is the unlock operation.</li>
//! </ul>
//! </div>
//!
//! The
//! \link lockpool_traits default traits class \endlink
//! implements \c insert_lock() and \c delete_lock()
//! by invoking the \c insert() and \c erase() methods of the \c container_t
//! (which is a \c std::set by default).
//!
//! The default lock pool traits class is sufficient in most situations.
//!
//! The \c starve flag is \c true by default.
//! When a lock gets removed, if another instance of the same
//! lock is waiting, it gets acquired immediately.
//  If \c starve is \c false, the
//! other lock waits until all older waiting locks are acquired, before this
//! locks gets acquired. A \c false value means that locks will
//! always be acquired strictly in the same order that the lock attempts
//! are made. The lock pool always waits until the oldest waiting lock
//! can be acquired, before checking the remaining locks.
//!
//! The default \c starve value of \c true provides the most opportunistic
//! locking. If the lock can be immediately acquired, it gets acquired, however
//! there are two disadvantages to this:
//!
//! - as discussed later, it is possible to define a lock pool where more than
//! one lock gets acquired simultaneously. If there's a waiting lock for
//! multiple values, it will will wait forever as long as other
//! waiting locks exist for individual values. As soon as one of the
//! lock values get unlocked, the other waiting lock gets acquired, ahead
//! of the older waiting lock, which is waiting until all lock values are
//! released.
//!
//! - each time a lock gets released, the entire list of waiting locks needs
//! to be searched for a waiting lock that can now be acquired. A large list
//! of waiting locks results in a slowdown.
//!
//! Setting \c starve to \c false speeds up lock processing, and eliminates
//! lock starvation. After a lock is removed, if the oldest waiting lock
//! cannot be acquired, no further processing takes place. However, it also
//! means that a newer attempt to create a lock may be placed in a waiting
//! status even though the lock is not presently acquired, because older
//! locks are waiting.
//!
//! \par Acquiring locks
//!
//! \code
//!
//! mylockpool_t::lockentry myLock=
//!        mylockpool->addLockSet("filelock", eventfd::create());
//!
//! \endcode
//!
//! \link lockpoolObj::addLockSet addLockSet() \endlink returns a reference
//! to an object that defines the lock specified by its first parameter.
//! The second parameter specifies an event file descriptor that's signaled
//! when the requested lock gets placed into the pool, if it cannot be
//! immediately placed. A new event file descriptor can
//! be created for each lock, or an event file descriptor may be created in
//! advance, and shared by multiple locks in the same, or different,
//! lock pools.
//!
//! \code
//!   while (!myLock->locked())
//!        myLock->getNotifyEvent()->event();
//! \endcode
//!
//! If the given lock is not blocked from being placed into the pool,
//! addLockSet() places the lock in the pool before returning.
//! \link lockpoolObj::lockentryObj::locked locked()\endlink indicates whether
//! the lock entry is held in the pool.
//! \link lockpoolObj::lockentryObj::getNotifyEvent() getNotifyEvent()\endlink
//! returns the event file descriptor, the second parameter passed to
//! addLockSet(), which is signaled when the lock gets placed into the pool.
//!
//! More than one lock entry may share the same event file descriptor.
//! Once an event is received, by the event file descriptor,
//! check each lock entry's locked() to determine which one of them are
//! now acquired by the lock pool.
//!
//! \note
//! The event file descriptor is not signaled if \c addLockSet() places the
//! lock into the pool immediately. If \c locked() returns true, no further
//! action is necessary. If \c locked() return false, wait for the event
//! file descriptor to get signaled, before checking again.
//!
//! Lock entries are references to reference-counted objects, which may be
//! freely passed around. When the last reference to a lock entry goes out of
//! scope, the lock is removed from the lock pool. This is the only way to
//! discard a lock. When a lock entry goes out of scope and gets destroyed,
//! it gets removed automatically from its lock pool, and another lock may
//! be acquired in its place, according to the \c starve setting and the
//! lock pool's traits,
//! and its event file descriptor gets signalled.
//!
//! If a lock entry goes out of scope while it was blocked, by another entry
//! for the same lock, from being held in the lock pool, the lock entry gets
//! removed from the lock pool's waiting list.
//!
//! \par Multiple lock entries
//!
//! It is possible to define a lock pool which locks multiple entries at once.
//! Using a \c std::string as an example of a lock entry, it is possible
//! to specify the lock identifier as a \c std::set of \c std::string objects.
//! This is the lock identifier, the \c lockid_t template parameter.
//!
//! However, the lock pool container is also a
//! \c std::set of \c std::string object. The process of acquiring a lock
//! places all \c std::string objects in the \c lockid_t into the lock pool
//! container, presuming that the lock pool container does not already contain
//! any one of the strings. If it does, the lock does not get acquired until
//! the lock pool container does not have any of the strings in the
//! \c lockid_t object, then they all get acquired at once.
//!
//! This is done by:
//!
//! - Specifying \c lockid_t as a \c std::set of \c T, where \c T is the
//! actual lock identifier.
//!
//! - Specifying \c compare_t as \c std::less&lt;T&gt; (or a comparable
//! functor).
//!
//! - Defining and specifying a suitable \c traits_t class.
//!
//! The custom \c traits_t contains the following members:
//!
//! - Defines \c container_t as \c std::set&lt;T, \c compare_t&gt; (or a comparable
//! container class).
//!
//! - Defines the appropriate \c insert_lock() and \c delete_lock().
//!
//! See multilockpool for an example.

template<typename lockid_t, typename compare_t=std::less<lockid_t>,
	 typename traits_t=lockpool_traits<lockid_t,
					   std::set<lockid_t, compare_t> >,
	 bool starve=true>
using lockpool=ref<lockpoolObj<lockid_t, compare_t, traits_t, starve>,
		   lockpoolBase<lockid_t, compare_t, traits_t, starve>
		   >;

//! A nullable reference pointer to a lockpool object.

//! \see lockpool

template<typename lockid_t, typename compare_t=std::less<lockid_t>,
	 typename traits_t=lockpool_traits<lockid_t,
					   std::set<lockid_t, compare_t> >,
	 bool starve=true>
using lockpoolptr=ptr<lockpoolObj<lockid_t, compare_t, traits_t, starve>,
		      lockpoolBase<lockid_t, compare_t, traits_t, starve>
		      >;

//! Refer to this class as %lockpool<>::%base or %lockpoolptr<>::%base

//! \see lockpool
//!

template<typename lockid_t, typename compare_t, typename traits_t, bool starve>
class lockpoolBase : public ptrref_base {
	//! My object implementation type

	typedef lockpoolObj<lockid_t, compare_t, traits_t, starve> impl_t;
public:
	//! Define the lockentry type

	//! The lockpoolObj object defines it. We alias it here.

	typedef typename impl_t::lockentry lockentry;

	//! Ditto for the lockentryptr type

	typedef typename impl_t::lockentryptr lockentryptr;

};

//! A pool of locks that use event file descriptors

//! \code
//!
//! typedef multilockpool<std::string>::lockpool_t pool_t;
//!
//! pool_t pool(pool_t::create());
//! std::set<std::string> keys;
//!
//! ...
//!
//! pool_t::lockentry myLock=pool->addLockSet(keys, eventfd);
//!
//! \endcode
//!
//! This template implements a
//! \ref lockpool "lock pool" where multiple locks are acquired
//! at once. The first template parameter is the lock identifier type, such as
//! a \c std::string.
//!
//! An optional second template parameter sets the \c starve flag
//! (see the lockpool documentation for more information), which defaults
//! to \c true.
//!
//! The resulting lock pool's addLockSet() method takes a %set of strings,
//! or a %set containing objects whose type is specified by the first
//! template parameter. An optional third template parameter specifies
//! a container to use in place of a %set. A set or a list is an acceptable
//! container. An optional fourth template parameter defines the ordering
//! functor for the first template parameter, defaulting to
//! \c std::less&lt;value_t&gt;.


template<typename value_t,
	 bool starve=true,
	 typename lockid_t=std::set<value_t>,
	 typename comp_t=std::less<value_t> >
class multilockpool {

	//! Traits class for the lock pool operations

	class multilockpool_traits {

	public:
		//! The actual lock pool is a set of the value types

		typedef std::set<value_t, comp_t> container_t;

		//! The insert_lock() implementation.

		static bool insert_lock(//! Lock identifiers
					const lockid_t &v,

					//! Existing locks
					container_t &s) noexcept
		{
			typename lockid_t::const_iterator b=v.begin(), p=b,
				e=v.end();

			while (p != e)
			{
				if (!s.insert(*p).second)
				{
					while (b != p)
						s.erase(*b++);
					return false;
				}
				++p;
			}
			return true;
		}

		//! The delete_lock() imlpementation.

		static void delete_lock(//! Lock identifiers
					const lockid_t &v,

					//! Existing locks.
					container_t &s) noexcept
		{
			typename lockid_t::const_iterator b=v.begin(),
				e=v.end();

			while (b != e)
			{
				s.erase(*b++);
			}
		}
	};

public:
	//! Multiple lock pool definition.

	typedef lockpool<lockid_t, comp_t,
			 multilockpool_traits,
			 starve> lockpool_t;
};

//! Define the \c sharedpool::sharedlock and \c sharedpool::uniquelock identifiers

class sharedpool_base {

public:
	//! A shared or a unique lock.

	typedef enum {
		sharedlock, //!< A shared lock
		uniquelock //!< A unique lock
	} lock_t;
};

//! The traits class for a shared lock pool

class sharedpool_traits : public sharedpool_base {

public:

	//! The actual lock pool is rather basic

	struct container_t {

		//! Number of shared locks acquired

		ssize_t sharedlockcnt;

		//! A unique lock is acquired

		bool uniquelockflag;

		//! Constructor
		container_t() noexcept;

		//! Destructor
		~container_t();
	};

	//! The insert_lock() implementation.

	static bool insert_lock(//! Lock identifier
				const lock_t &v,

				//! Existing locks
				container_t &s) noexcept;

	//! The delete_lock() imlpementation.

	static void delete_lock(//! Lock identifiers
				const lock_t &v,

				//! Existing locks.
				container_t &s) noexcept;
};

template<bool starve> class sharedpoolBase;

//! A shared lock pool

//! This is a specialization of
//! \ref lockpool "lockpool" that implements shared locks.
//! The shared lock has the same semantics as shared,
//! except that the locking operation
//! carries lockpool semantics: it's always nonblocking, and an event file
//! descriptor signals lock acquisition.
//!
//! \code
//! typedef sharedpool<> sharedpool_t;
//!
//! sharedpool_t sharedpool(sharedpool_t::create());
//!
//! sharedpool_t::lockentry rlock(sharedpool->addLockSet(sharedpool_t::base::sharedlock,
//!                                                  eventfd::create());
//!
//! sharedpool_t::lockentry rlock(sharedpool->addLockSet(sharedpool_t::base::uniquelock,
//!                                                  eventfd::create());
//! \endcode
//!
//! The sharedpool template
//! defines a reference to a reference-counted lock pool type.
//! sharedpool takes one template parameter, a bool flag, that sets the
//! \c starve flag of the underlying sharedpool implementation. The default
//! \c starve flag is \c false. As soon as a unique lock is placed on a
//! a waiting list for locking, any additional shared lock attempts are queued
//! up behind it, until all existing shared locks go out of scope, and the
//! unique lock is acquired. Specifying \c starve as true means that new
//! shared locks always get acquired as long as there's one or more existing
//! locks in the lock pool, and pending unique locks starve until all
//! shared locks go out of scope.
//!
//! \note
//! This is a template alias for a \ref ref "ref" to an lockpoolObj.

template<bool starve=false>
using sharedpool=ref<lockpoolObj<sharedpool_traits::lock_t,
				 std::less<sharedpool_traits::lock_t>,
				 sharedpool_traits, starve>,
		     sharedpoolBase<starve> >;

//! A nullable reference pointer to a shared lock pool.

//! \see sharedpool

template<bool starve=false>
using sharedpoolptr=ptr<lockpoolObj<sharedpool_traits::lock_t,
				    std::less<sharedpool_traits::lock_t>,
				    sharedpool_traits, starve>,
			sharedpoolBase<starve> >;

//! \c sharedpool<bool>::base and \c sharedpoolptr<bool>::base

//! \see lockpool
//!

template<bool starve>
class sharedpoolBase :
	public lockpoolBase<sharedpool_traits::lock_t,
			    std::less<sharedpool_traits::lock_t>,
			    sharedpool_traits, starve>,
	public sharedpool_base {
};

//! The traits class that implements the mutually-exclusive locking strategy.

//! \see mutexpool

template<typename mutex_t,
	 typename compare_t,
	 bool starve> class mutexpool_traits {
 public:

	//! The official container of locks. It's boring.

	class container_t {

	public:
		//! What's currently locked
		mutex_t current_lock_type;

		//! How many times its locked
		size_t current_lock_cnt;

		//! Comparison operator
		compare_t comp;

		//! Constructor
		container_t() : current_lock_cnt(0) {}

		//! Destructor
		~container_t() {}
	};

	//! The insert_lock() implementation.

	static bool insert_lock(//! Lock identifier
				const mutex_t &value,

				//! Existing locks
				container_t &c)
	{
		if (c.current_lock_cnt == 0)
			c.current_lock_type=value;

		if (c.comp(c.current_lock_type, value) ||
		    c.comp(value, c.current_lock_type))
			return false;

		++c.current_lock_cnt;
		return true;
	}

	//! The delete_lock() implementation.

	static void delete_lock(//! Lock identifier
				const mutex_t &value,

				//! Existing locks
				container_t &c)
	{
		--c.current_lock_cnt;
	}
};

//! A mutually-exclusive lock pool

//! This is a specialization of
//! \ref lockpool "lockpool" that implements mutually-exclusive locks.
//!
//! This template class takes three parameters.
//!
//! <div style="margin-left: 3em">
//! <ul>
//! <li>\c mutex_t - the mutually-exclusive lock type, such as \c std::string
//! or \c bool.</li>
//!
//! <li>\c compare_t - A comparison function for object lock types. Defaults to
//! \c std::less&lt;mutex_t&gt;.</li>
//!
//! <li>\c bool starve - a boolean flag that selects whether older waiting
//! locks can be starved by newer waiting locks. This gets passed through
//! to the \ref lockpool "lockpool" template class.
//! </li>
//! </ul>
//! </div>
//!
//! Example:
//!
//! \code
//! typedef INSERT_LIBX_NAMESPACE::mutexpool<> blocklock_t;
//!
//! typedef blocklock_t::lockentry lockentry_t;
//! \endcode
//!
//! The \c mutexpool helper template class specializes a
//! \ref lockpool "lockpool" that allows only one value to be locked at the
//! same time, but the same value may be locked multiple times. Attempts to
//! place another lock value into the lock pool get blocked, until all existing
//! locks, of the same value, are released.
//!
//! The above example, using \c bools,
//! is convenient implementation of a "blocking lock" for
//! some unspecified resource. Using this resource requires obtaining a "false"
//! lock value. This allows multiple concurrent usage. Concurrent existence of
//! multiple "false" values is allowed.
//!
//! Acquiring a "true" lock value, in this example, effectively blocks usage
//! of the protected resource. The "true" lock value gets acquired only after
//! all existing "false" values no longer exist, and future "false" lock
//! values won't get acquired until the "true" lock values are gone. Multiple
//! "true" lock values may exist, blocking access to the resource, and
//! "false" lock values won't get acquired until the last "true" lock is gone.
//!
//! In this example, the "true" and "false" lock values are symmetric. Each
//! one blocks the other lock value. The first parameter to this template
//! class can be something other than a \c bool, resulting in the same
//! semantics: once the first lock value is acquired, only same lock values
//! may be acquired additionally; all other lock values are blocked until
//! all existing lock values are gone.
//!
//! \note
//! This is a template alias for a specialized \ref lockpool "lockpool".

template<typename mutex_t=bool,
	 typename compare_t=std::less<mutex_t>,
	 bool starve=true>
using mutexpool=lockpool<mutex_t, compare_t,
			 mutexpool_traits<mutex_t, compare_t, starve>,
			 starve>;

//! A nullable reference pointer to a mutually-exclusive pool object.

//! \see mutexpool

template<typename mutex_t=bool,
	 typename compare_t=std::less<mutex_t>,
	 bool starve=true>
using mutexpoolptr=lockpoolptr<mutex_t, compare_t,
			       mutexpool_traits<mutex_t, compare_t, starve>,
			       starve>;

// Instantiate the templates in the library

extern template class mutexpool_traits<bool, std::less<bool>, true>;
extern template class mutexpool_traits<bool, std::less<bool>, false>;

extern template class lockpoolObj<bool, std::less<bool>,
				  mutexpool_traits<bool, std::less<bool>, true>,
				  true>;
extern template class lockpoolObj<bool, std::less<bool>,
				  mutexpool_traits<bool, std::less<bool>, false>,
				  false>;

extern template class lockpoolObj<sharedpool_traits::lock_t,
				  std::less<sharedpool_traits::lock_t>,
				  sharedpool_traits, true>;

extern template class lockpoolObj<sharedpool_traits::lock_t,
				  std::less<sharedpool_traits::lock_t>,
				  sharedpool_traits, false>;

#if 0
{
#endif
}
#endif
