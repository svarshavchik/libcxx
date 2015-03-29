/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_vipobjdebug_H
#define x_vipobjdebug_H

#include <x/namespace.h>
#include <x/rwmutexdebug.H>
#include <x/vipobj.H>
#include <x/logger.H>
#include <x/exception.H>
#include <x/property_valuefwd.H>
#include <map>
#include <mutex>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Base class for \ref vipobjdebug "vipobjdebug".

class vipobjdebug_base {

	//! The \c INSERT_LIBX_NAMESPACE::vipobjdebug_base::abort property

	//! If set to true, abort() gets called instead of throwing an
	//! %exception.

	static property::value<bool> abort_prop LIBCXX_HIDDEN;

protected:

	//! The logger object

	LOG_CLASS_SCOPE;

public:

	//! Internal function
	static std::string deadlock_detected_msg() LIBCXX_INTERNAL;

	//! A conflicting lock has been detected, throw an %exception

	static void deadlock_detected(const std::string &lock1,
				      const std::string &lock2)
		__attribute__((noreturn));

};

//! Debugging \ref vipobj "very important objects".

//! Use this class in place of \ref vipobj "vipobj" to add additional
//! checks for locking order that might lead to deadlocks.
//!
//! This template class defines the same interface as \ref vipobj "vipobj",
//! and adds an additional check: the same thread cannot acquire an update
//! lock or a handler lock on a very important object if it already holds
//! a read lock or a write lock, on the same object. Although normally
//! this works, it might lead to deadlocks in situations where one code
//! path, for example, acquires a read lock, then a handler lock for the
//! purpose of installing a handler (seems innocently enough), and another
//! thread acquires an update lock first, then attempts a write lock in order
//! to update the very important object.
//! 
//! This template class throws an %exception in the following circumstances:
//!
//! - the same thread attempts to acquire a handler lock or an update lock
//! while holding a read lock or a write lock on the same very important object.
//!
//! - the same thread attempts to acquire both a read lock and a write lock,
//! or a handler lock and an update lock on the same object. Normally this
//! results in an immediate deadlock, which is fairly obvious; but setting
//! the log level to the trace level gives convenient backtraces where the
//! conflicting locks were acquired.
//!
//! This template class can only be used if locks are held on the stack
//! (which shuold ordinarily be the case), and
//! may not give reliable results if locks are held on the heap, and juggled
//! between different threads.
//!
//! If the \c INSERT_LIBX_NAMESPACE::vipobjdebug_base::abort property
//! is set to true, abort() gets called instead of throwing an
//! %exception when conflicting locking gets detected.
//!
//! This template adds significant overhead, and should only be used for
//! debugging purposes.

template<typename obj_type,
	 typename callback_arg_type=const obj_type &,
	 bool trap_exceptions=false>
class vipobjdebug : private vipobj<obj_type, callback_arg_type,
				   trap_exceptions, rwmutexdebug> {

	//! Identify locks by type, so that conflicting locks may be identified
	typedef int lock_t;

	//! Identifier for a read lock
	static const int readlock_t=0;

	//! Identifier for a write lock
	static const int writelock_t=1;

	//! Identifier for an update lock
	static const int updatelock_t=2;

	//! Identifier for a handler lock
	static const int handlerlock_t=3;

	//! A record of an acquired lock
	class lockrecord_t {

	public:
		//! What kind of a lock it is
		lock_t locktype;

		//! Which thread acquired this lock
		tid_t threadid;

		//! Constructor
		lockrecord_t(lock_t locktypearg, tid_t threadidarg) noexcept
		: locktype(locktypearg), threadid(threadidarg)
		{
		}

		//! Destructor
		~lockrecord_t() noexcept
		{
		}

		//! Associative operation for the lock identifier
		bool operator<(const lockrecord_t &o) const noexcept
		{
			if ( locktype < o.locktype ) return true;
			if ( o.locktype < locktype ) return false;

			return threadid < o.threadid;
		}
	};

	//! A container that maintains a record of acquired locks
	typedef std::multimap<lockrecord_t, std::string> locks_t;

	//! Which locks have been acquired
	locks_t locks;

	//! A mutex for protecting \ref locks "locks".

	std::mutex locksmutex;

	template<lock_t> class deadlocker;

public:

	template<lock_t> friend class deadlocker;

	//! The superclass
	typedef vipobj<obj_type, callback_arg_type,
		       trap_exceptions, rwmutexdebug> vipobj_t;

	//! This template class instance
	typedef vipobjdebug<obj_type> vipobjdebug_t;

	//! Default constructor
	vipobjdebug() {}

	//! Constructor with one argument
	template<typename init_type>
	vipobjdebug(const init_type &objArg)
		: vipobj_t(objArg) {}

	//! Destructor
	~vipobjdebug() noexcept {}

private:

	//! Throw an %exception if a conflicting lock exists

	//! The subclass derives from this class. If the constructor finds
	//! an existing lock of the type given by the template parameter,
	//! the constructor throws an %exception.
	template<lock_t n> class deadlocker {

	public:
		//! Constructor
		deadlocker(vipobjdebug_t &v)
		{
			std::lock_guard<std::mutex> lock(v.locksmutex);

			typename locks_t::iterator iter=
				v.locks.find(lockrecord_t(n, gettid()));

			if (iter == v.locks.end())
				return;

			exception e;

			vipobjdebug_base::deadlock_detected(iter->second,
							    e->backtrace);
		}

		//! Destructor
		~deadlocker() noexcept {}
	};

	//! Record lock instantiation

	//! The constructor adds a record in the \ref locks "locks" table
	//! that the lock of the type given by the template parameter has
	//! been created by this thread. The destructor removes it.

	template<lock_t n> class lockentry {

		//! The very important object
		vipobjdebug_t &lockpool;

		//! The iterator of the entry for this lock in the locks table.
		typename locks_t::iterator iter;

	public:
		//! Constructor
		lockentry(vipobjdebug_t &lockpoolArg)
			: lockpool(lockpoolArg)
		{
			exception e;

			std::lock_guard<std::mutex> lock(lockpool.locksmutex);

			iter=lockpool.locks.insert(std::make_pair
						   (lockrecord_t(n, gettid()),
						    e->backtrace));
		}

		//! Destructor
		~lockentry() noexcept
		{
			std::lock_guard<std::mutex> lock(lockpool.locksmutex);

			lockpool.locks.erase(iter);
		}
	};

public:

	//! A wrapper for a read lock

	//! The read lock conflicts with a write lock on the same
	//! very important object.

	class readlock :
		private deadlocker<writelock_t>,
		private lockentry<readlock_t>,
		public vipobj_t::readlock {

	public:

		//! Constructor
		template<typename ...Args>
		readlock(vipobjdebug_t &obj, Args && ...args)
			: deadlocker<writelock_t>(obj),
			  lockentry<readlock_t>(obj),
			  vipobj_t::readlock(obj, std::forward<Args>(args)...)
		{
		}

		//! Destructor
		~readlock() noexcept {}

		using vipobj_t::readlock::operator*;
		using vipobj_t::readlock::operator->;

	};

	//! A wrapper for a write lock

	//! The write lock conflicts with a read lock on the same
	//! very important object.

	class writelock :
		private deadlocker<readlock_t>,
		private lockentry<writelock_t>,
		public vipobj_t::writelock {

	public:
		//! Constructor
		template<typename ...Args>
		writelock(vipobjdebug_t &obj, Args && ...args)
			: deadlocker<readlock_t>(obj),
			  lockentry<writelock_t>(obj),
			  vipobj_t::writelock(obj, std::forward<Args>(args)...)
		{
		}

		//! Destructor

		~writelock() noexcept {}

		using vipobj_t::writelock::operator*;
		using vipobj_t::writelock::operator->;
	};

	//! A wrapper for an update lock

	//! The update lock conflicts with read, write, and handler locks
	//! on the same very important object

	class updatelock :
		private deadlocker<handlerlock_t>,
		private deadlocker<readlock_t>,
		private deadlocker<writelock_t>,
		public vipobj_t::updatelock {

	public:
		//! Constructor
		template<typename ...Args>
		updatelock(vipobjdebug_t &obj, Args && ...args)
			: deadlocker<handlerlock_t>(obj),
			  deadlocker<readlock_t>(obj),
			  deadlocker<writelock_t>(obj),
			  vipobj_t::updatelock(obj,
					       std::forward<Args>(args)...)
		{
		}

		//! Destructor
		~updatelock() noexcept {}
	};

	//! A wrapper for a handler lock

	//! The handler lock conflicts with read, write, and update locks
	//! on the same very important object

	class handlerlock :
		private deadlocker<updatelock_t>,
		private deadlocker<readlock_t>,
		private deadlocker<writelock_t>,
		public vipobj_t::handlerlock {

	public:
		//! Constructor
		template<typename ...Args>
		handlerlock(vipobjdebug_t &obj, Args && ...args)
			: deadlocker<updatelock_t>(obj),
			  deadlocker<readlock_t>(obj),
			  deadlocker<writelock_t>(obj),
			  vipobj_t::handlerlock(obj, std::forward<Args>(args)...)
		{
		}

		//! Destructor
		~handlerlock() noexcept {}
	};
};

#if 0
{
#endif
}
#endif

