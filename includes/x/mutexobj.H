/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mutexobj_H
#define x_mutexobj_H

#include <x/obj.H>
#include <x/mutexfwd.H>
#include <x/mlockfwd.H>
#include <x/namespace.h>

#include <condition_variable>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class condObj;

//! A thread-safe muteximpl

class mutexObj : virtual public obj {

	//! Mutex has been acquired. Protected by objmutex
	bool acquired;

	//! For internal signaling.
	std::condition_variable cond;

public:

	//! Create a new mutex
	mutexObj();

	//! Default destructor
	~mutexObj();

	friend class mlockObj;
	friend class condObj;

	//! Acquire a lock, however long it takes.

	mlock lock();

	//! Acquire a lock, if possible

	mlockptr trylock();

	//! Try a lock

	//! If the lock was acquired, return an mlock, otherwise a null.

	template<typename timeout_type>
	mlockptr wait_until(const timeout_type &t)
	{
		mlockptr ptr;

		std::unique_lock<std::mutex> lock(objmutex);

		while (acquired)
		{
			if (cond.wait_until(lock, t) ==
			    std::cv_status::timeout)
				break;
		}

		if (!acquired)
			ptr=acquire();

		return ptr;
	}

private:
	//! Internal: the mutex is available

	mlock acquire();
};

#if 0
{
#endif
}
#endif
