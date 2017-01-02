/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef rwmutexdebug_H
#define rwmutexdebug_H

#include <x/namespace.h>
#include <x/rwmutex.H>
#include <x/mpobj.H>
#include <x/logger.H>
#include <thread>
#include <set>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A debug version of rwmutexdebug

//! Throws an exception if the same thread tries to obtain two locks on the
//! same rwmutex.

class rwmutexdebug : public rwmutex {

	//! The class logger
	LOG_CLASS_SCOPE;

	//! Containing of per-thread locks.

	typedef mpobj<std::set<std::thread::id> > lockpool_t;

	//! Per-thread locks.
	lockpool_t lockpool;

	//! Bad locking detected.
	static void oops() __attribute__((noreturn));

	//! Base class implements checking of single locking/unlocking.

	class debuglockbase {

		//! The thread pool container
		lockpool_t &lockpool;

	protected:

		//! This thread wants to own one of the locks
		void own();

		//! This thread is about to release its lock
		void unown();

		//! Constructor
		debuglockbase(lockpool_t &lockpoolArg);
	};

	//! Template for wrapping rmutex or wmutex

	template<typename lock_t> class debuglock : private debuglockbase {

		//! The underlying rmutex or wmutex
		lock_t &l;

	public:

		//! Constructor
		debuglock(lockpool_t &lockpool, lock_t &lArg)
			: debuglockbase(lockpool), l(lArg)
		{
		}

		//! Wrap lock()
		void lock()
		{
			own();
			try {
				l.lock();
			} catch (...)
			{
				unown();
				throw;
			}
		}

		//! Wrap unlock()
		bool try_lock()
		{
			own();
			try {
				if (!l.try_lock())
				{
					unown();
					return false;
				}
			} catch (...)
			{
				unown();
				throw;
			}
			return true;
		}

		//! Wrap unlock()
		void unlock()
		{
			unown();
			l.unlock();
		}

		//! Wrap try_lock_for()
		template<typename timeout_type>
		bool try_lock_for(const timeout_type &t)
		{
			own();
			try {
				if (!l.try_lock_for(t))
				{
					unown();
					return false;
				}
			} catch (...)
			{
				unown();
				throw;
			}
			return true;
		}

		//! Wrap try_lock_until()
		template<typename timeout_type>
		bool try_lock_until(const timeout_type &t)
		{
			own();
			try {
				if (!l.try_lock_until(t))
				{
					unown();
					return false;
				}
			} catch (...)
			{
				unown();
				throw;
			}
			return true;
		}
	};

public:

	//! Container for the read mutex wrapper
	typedef debuglock<rwmutex::rmutex> rmutex;

	//! Container for the write mutex wrapper
	typedef debuglock<rwmutex::wmutex> wmutex;

	//! The wrapper read mutex
	rmutex r;

	//! The wrapper write mutex
	wmutex w;

	//! Constructor
	rwmutexdebug();

	//! Destructor
	~rwmutexdebug();
};

#if 0
{
#endif
}
#endif
