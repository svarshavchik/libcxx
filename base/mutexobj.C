/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mutex.H"
#include "x/cond.H"
#include "x/exception.H"
#include <iostream>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

mutexObj::mutexObj() : acquired{false}
{
}
#if 0
mutexObj::mutexObj(const cond_attr &attr) : acquired(false),
					    cond(attr)
{
}
#endif

mutexObj::~mutexObj()
{
}

mlockObj::mlockObj(const mutex &m)
	: m{m}
{
	m->acquired=true;
}

mlockObj::~mlockObj()
{
	std::lock_guard<std::mutex> lock{m->objmutex};

	m->acquired=false;
	m->cond.notify_all();
}

mlock mutexObj::lock()
{
	std::unique_lock<std::mutex> lock(objmutex);

	while (acquired)
		cond.wait(lock);

	return acquire();
}

mlock mutexObj::acquire()
{
	return mlock::create(ref{this});
}

mlockptr mutexObj::trylock()
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
