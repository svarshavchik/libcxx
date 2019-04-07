/*
** Copyright 2012-2019 Double Precision, Inc.
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

sharedlockObj::sharedlockObj()=default;

sharedlockObj::~sharedlockObj()=default;

sharedlockObj::sharedObj::sharedObj(const sharedlock &lock,
				    mpcobj<lock_info_t>::lock &l)
	: lock{lock}
{
	++l->shared_lockers;
}

sharedlockObj::sharedObj::~sharedObj()
{
	mpcobj<lock_info_t>::lock l{lock->lock_info};

	--l->shared_lockers;

	l.notify_all();
}

sharedlockObj::uniqueObj::uniqueObj(const sharedlock &lock,
				    mpcobj<lock_info_t>::lock &l)
	: lock{lock}
{
	l->exclusively_locked=true;
}

sharedlockObj::uniqueObj::~uniqueObj()
{
	mpcobj<lock_info_t>::lock l{lock->lock_info};

	l->exclusively_locked=false;

	l.notify_all();
}

sharedlock::base::shared sharedlockObj::create_shared()
{
	mpcobj<lock_info_t>::lock l{lock_info};

	l.wait([&]
	       {
		       return l->ok_to_lock_shared();
	       });

	return shared::create(ref{this}, l);
}

sharedlock::base::sharedptr sharedlockObj::try_shared()
{
	sharedptr ptr;

	mpcobj<lock_info_t>::lock l{lock_info};

	if (l->ok_to_lock_shared())
	{
		ptr=shared::create(ref{this}, l);
	}
	return ptr;
}

sharedlock::base::unique sharedlockObj::create_unique()
{
	mpcobj<lock_info_t>::lock l{lock_info};

	l.wait([&]
	       {
		       return l->ok_to_lock_exclusively();
	       });

	return unique::create(ref{this}, l);
}


sharedlock::base::uniqueptr sharedlockObj::try_unique()
{
	uniqueptr ptr;

	mpcobj<lock_info_t>::lock l{lock_info};

	if (l->ok_to_lock_exclusively())
	{
		ptr=unique::create(ref{this}, l);
	}
	return ptr;
}

#if 0
{
#endif
}
