/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/destroycallbackflagwait4.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

destroyCallbackFlagWait4Obj::destroyCallbackFlagWait4Obj(const ref<obj>
							 &other_obj)
	: flag(destroyCallbackFlag::create())
{
	auto f=flag;

	other_obj->ondestroy([f] { f->destroyed(); });
}

destroyCallbackFlagWait4Obj::~destroyCallbackFlagWait4Obj() noexcept
{
}

void destroyCallbackFlagWait4Obj::destroyed()
{
	flag->wait();
}

#if 0
{
#endif
}
