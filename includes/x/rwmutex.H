/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef rwmutex_H
#define rwmutex_H

#include <x/namespace.h>
#include <x/notcopyableobj.H>
#include <mutex>
#include <condition_variable>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Readers/writer mutexes

//! This class instantiates two related mutexes, a readers mutex, and a writer
//! mutex.

class rwmutex : public notcopyableObj {

public:

	class rmutex;

	//! The writer half of readers/writer mutex pair

	//! This is a \c TimedLockable mutex that locks when there are no
	//! other execution threads that own this mutex or its related readers
	//! mutex.

	class wmutex : public notcopyableObj {

		//! Number of readers that have the readers/writer mutex locked.
		size_t			nreaders;

		//! Whether a writer has the readers/writer mutex locked.

		//! \c wlocked also gets set when there's an attempted writer
		//! lock that's waiting for all the reader locks to release.
		bool			wlocked;

		//! An ordinary mutex that protects nreaders and wlocked
		std::mutex		mutex;

		//! A condition variable that's notified when a reader or a writer unlocks the readers/writer mutex.
		std::condition_variable cond;

		//! Private constructor
		wmutex();

	public:

		friend class rmutex;
		friend class rwmutex;

		//! The \c BasicLockable requirement
		void lock();

		//! The \c BasicLockable requirement

		void unlock();

		//! The \c Lockable requirement
		bool try_lock();

		template<typename timeout_type>
		bool try_lock_for(const timeout_type &rel_time);

		template<typename timeout_type>
		bool try_lock_until(const timeout_type &abs_time);
	};

	//! The reader half of readers/writer mutex pair

	//! This is a \c TimedLockable mutex that locks when there are no
	//! other execution threads that own the related writer mutex.

	class rmutex : public notcopyableObj {

		//! The related writer mutex
		wmutex &w;

		//! Private constructor
		rmutex(wmutex &wArg);

	public:
		friend class rwmutex;

		//! The \c BasicLockable requirement
		void lock();

		//! The \c BasicLockable requirement
		void unlock();

		//! The \c Lockable requirement
		bool try_lock();

		template<typename timeout_type>
		bool try_lock_for(const timeout_type &rel_time);

		template<typename timeout_type>
		bool try_lock_until(const timeout_type &abs_time);
	};

	//! The writer mutex
	wmutex w;

	//! The reader mutex
	rmutex r;

	//! Constructor
	rwmutex();

	//! Destructor
	~rwmutex();
};


//! The \c TimedLockable requirement

template<typename timeout_type>
bool rwmutex::wmutex::try_lock_for(const timeout_type &rel_time)
{
	std::unique_lock<std::mutex> lock(mutex);

	bool captured=false;

	bool stat;

	try {
		stat=cond.wait_for(lock, rel_time,
				   [&captured, this]
				   {
					   if (!wlocked)
						   captured=wlocked=true;

					   return captured && nreaders == 0;
				   });
	} catch (...) {
		if (captured)
		{
			wlocked=false;
			cond.notify_all();
		}
		throw;
	}

	if (!stat)
	{
		if (captured)
		{
			wlocked=false;
			cond.notify_all();
		}
	}
	return stat;
}

//! The \c TimedLockable requirement

template<typename timeout_type>
bool rwmutex::wmutex::try_lock_until(const timeout_type &abs_time)
{
	std::unique_lock<std::mutex> lock(mutex);

	bool captured=false;

	bool stat;

	try {
		stat=cond.wait_until(lock, abs_time,
				     [&captured, this]
				     {
					     if (!wlocked)
						     captured=wlocked=true;

					     return captured && nreaders == 0;
				     });
	} catch (...) {
		if (captured)
		{
			wlocked=false;
			cond.notify_all();
		}
		throw;
	}

	if (!stat)
	{
		if (captured)
		{
			wlocked=false;
			cond.notify_all();
		}
	}
	return stat;
}

//! The \c TimedLockable requirement

template<typename timeout_type>
bool rwmutex::rmutex::try_lock_for(const timeout_type &rel_time)
{
	std::unique_lock<std::mutex> lock(w.mutex);

	if (!w.cond.wait_for(lock, rel_time,
			     [this]
			     {
				     return w.wlocked == false;
			     }))
		return false;

	++w.nreaders;
	return true;
}

//! The \c TimedLockable requirement

template<typename timeout_type>
bool rwmutex::rmutex::try_lock_until(const timeout_type &abs_time)
{
	std::unique_lock<std::mutex> lock(w.mutex);

	if (!w.cond.wait_until(lock, abs_time,
			       [this]
			       {
				       return w.wlocked == false;
			       }))
		return false;
	++w.nreaders;
	return true;
}

#if 0
{
#endif
}
#endif
