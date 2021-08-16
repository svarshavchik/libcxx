/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/option_mutexobj.H"
#include "x/option_valuebaseobj.H"

namespace LIBCXX_NAMESPACE::option {
#if 0
};
#endif

mutexObj::mutexObj() noexcept: value(false)
{
}

mutexObj::~mutexObj()
{
}

mutexObj &mutexObj::add(const ref<valuebaseObj> &valueRef) noexcept
{
	valueRef->value_mutex= this;
	return *this;
}

#if 0
{
#endif
}