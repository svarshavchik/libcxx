/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/rwlock.H"
#include "x/exception.H"
#include <iostream>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

rwlockObj::rwlockObj()
{
}

rwlockObj::~rwlockObj()
{
}

rwlockObj::rlockObj::rlockObj()
{
}

rwlockObj::rlockObj::rlockObj(const rwlock &lockArg)
	: lock(lockArg), rlock(lockArg->rw.r, std::defer_lock_t())
{
}

rwlockObj::rlockObj::~rlockObj()
{
}

rwlockObj::wlockObj::wlockObj()
{
}

rwlockObj::wlockObj::wlockObj(const rwlock &lockArg)
	: lock(lockArg), wlock(lockArg->rw.w, std::defer_lock_t())
{
}

rwlockObj::wlockObj::~wlockObj()
{
}

rwlock::base::rlock rwlockObj::readlock()
{
	rlock r=rlock::create(rwlock(this));

	r->rlock.lock();

	return r;
}

rwlock::base::rlockptr rwlockObj::try_readlock()
{
	rlockptr ptr;

	rlock r=rlock::create(rwlock(this));

	if (r->rlock.try_lock())
		ptr=r;

	return ptr;
}

rwlock::base::wlock rwlockObj::writelock()
{
	wlock w=wlock::create(rwlock(this));

	w->wlock.lock();

	return w;
}


rwlock::base::wlockptr rwlockObj::try_writelock()
{
	wlockptr ptr;

	wlock w=wlock::create(rwlock(this));

	if (w->wlock.try_lock())
		ptr=w;

	return ptr;
}

#if 0
{
#endif
}
