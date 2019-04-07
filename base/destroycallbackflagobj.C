/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/destroy_callback.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

destroy_callbackObj::destroy_callbackObj() noexcept
{
	*mpcobj<bool>::lock(flag)=false;
}

destroy_callbackObj::~destroy_callbackObj()
{
}

void destroy_callbackObj::destroyed()
{
	mpcobj<bool>::lock lock(flag);

	*lock=true;
	lock.notify_all();
}

void destroy_callbackObj::wait()
{
	mpcobj<bool>::lock lock(flag);

	lock.wait([&lock] { return *lock; });
}

destroy_callbackBase::guard::guard()
{
}

destroy_callbackBase::guard::~guard()
{
	while (!callbacks.empty())
	{
		callbacks.front()->wait();
		callbacks.pop_front();
	}
}

void destroy_callbackBase::guard::add(const x::ref<x::obj> &obj)
{
	auto cb=destroy_callback::create();

	obj->ondestroy([cb] { cb->destroyed(); });
	callbacks.push_back(cb);
}

#if 0
{
#endif
}
