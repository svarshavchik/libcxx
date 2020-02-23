/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/option_mutexobj.H"
#include "x/option_valuebaseobj.H"

namespace LIBCXX_NAMESPACE {
	namespace option {
#if 0
	};
};
#endif

mutexObj::mutexObj() noexcept: value(false)
{
}

mutexObj::~mutexObj()
{
}

mutexObj &mutexObj::add(const ptr<valuebaseObj> &valueRef) noexcept
{
	valueRef->value_mutex= this;
	return *this;
}

#if 0
{
	{
#endif
	}
}
