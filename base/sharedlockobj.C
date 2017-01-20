/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sharedlock.H"
#include "x/exception.H"
#include <iostream>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

sharedlockObj::sharedlockObj()
{
}

sharedlockObj::~sharedlockObj()
{
}

sharedlockObj::sharedObj::sharedObj()
{
}

sharedlockObj::sharedObj::sharedObj(const sharedlock &lockArg)
	: lock(lockArg), shared(lockArg->rw, std::defer_lock_t())
{
}

sharedlockObj::sharedObj::~sharedObj()
{
}

sharedlockObj::uniqueObj::uniqueObj()
{
}

sharedlockObj::uniqueObj::uniqueObj(const sharedlock &lockArg)
	: lock(lockArg), unique(lockArg->rw, std::defer_lock_t())
{
}

sharedlockObj::uniqueObj::~uniqueObj()
{
}

sharedlock::base::shared sharedlockObj::create_shared()
{
	shared r=shared::create(sharedlock(this));

	r->shared.lock();

	return r;
}

sharedlock::base::sharedptr sharedlockObj::try_shared()
{
	sharedptr ptr;

	shared r=shared::create(sharedlock(this));

	if (r->shared.try_lock())
		ptr=r;

	return ptr;
}

sharedlock::base::unique sharedlockObj::create_unique()
{
	unique w=unique::create(sharedlock(this));

	w->unique.lock();

	return w;
}


sharedlock::base::uniqueptr sharedlockObj::try_unique()
{
	uniqueptr ptr;

	unique w=unique::create(sharedlock(this));

	if (w->unique.try_lock())
		ptr=w;

	return ptr;
}

#if 0
{
#endif
}
