/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/destroy_callback_wait4.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

destroy_callback_wait4Obj::destroy_callback_wait4Obj(const ref<obj>
							 &other_obj)
	: flag(destroy_callback::create())
{
	auto f=flag;

	other_obj->ondestroy([f] { f->destroyed(); });
}

destroy_callback_wait4Obj::~destroy_callback_wait4Obj() noexcept
{
}

void destroy_callback_wait4Obj::destroyed()
{
	flag->wait();
}

#if 0
{
#endif
}
