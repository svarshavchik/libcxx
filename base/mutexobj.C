/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mutex.H"
#include "x/exception.H"
#include <iostream>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

mutexObj::mutexObj() : acquired(false)
{
}
#if 0
mutexObj::mutexObj(const cond_attr &attr) : acquired(false),
					    cond(attr)
{
}
#endif

mutexObj::~mutexObj() noexcept
{
}

void mutexObj::unlock() noexcept
{
	std::lock_guard<std::mutex> lock(objmutex);

	acquired=false;
	cond.notify_all();
}

mutexObj::mlockObj::mlockObj()
{
}

mutexObj::mlockObj::~mlockObj() noexcept
{
	if (!m.null())
		m->unlock();
}

mutex::base::mlock mutexObj::lock()
{
	std::unique_lock<std::mutex> lock(objmutex);

	while (acquired)
		cond.wait(lock);

	return acquire();
}

mutex::base::mlock mutexObj::acquire()
{
	mlock lock=mlock::create();

	acquired=true;
	lock->m=mutexptr(this);

	return lock;
}

mutex::base::mlockptr mutexObj::trylock()
{
	mlockptr ptr;

	std::lock_guard<std::mutex> lock(objmutex);

	if (!acquired)
		ptr=acquire();

	return ptr;
}

#if 0
{
#endif
}
