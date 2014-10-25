/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/destroycallbackflag.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

destroyCallbackFlagObj::destroyCallbackFlagObj() noexcept
{
	*mpcobj<bool>::lock(flag)=false;
}

destroyCallbackFlagObj::~destroyCallbackFlagObj() noexcept
{
}

void destroyCallbackFlagObj::destroyed()
{
	mpcobj<bool>::lock lock(flag);

	*lock=true;
	lock.notify_all();
}

void destroyCallbackFlagObj::wait()
{
	mpcobj<bool>::lock lock(flag);

	lock.wait([&lock] { return *lock; });
}

destroyCallbackFlagBase::guard::guard()
{
}

destroyCallbackFlagBase::guard::~guard() noexcept
{
	while (!callbacks.empty())
	{
		callbacks.front()->wait();
		callbacks.pop_front();
	}
}

void destroyCallbackFlagBase::guard::add(const x::ref<x::obj> &obj)
{
	auto cb=destroyCallbackFlag::create();

	obj->ondestroy([cb] { cb->destroyed(); });
	callbacks.push_back(cb);
}

#if 0
{
#endif
}
