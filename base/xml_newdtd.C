/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "xml_internal.h"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

newdtdObj::newdtdObj()
{
}

newdtdObj::~newdtdObj() noexcept
{
}

newdtdimplObj::newdtdimplObj(const subset_impl_t &subset_implArg,
			     const ref<impldocObj> &implArg,
			     const doc::base::writelock &lockArg)
	: dtdimplObj(subset_implArg, implArg, lockArg), lock(lockArg)
{
}

newdtdimplObj::~newdtdimplObj() noexcept
{
}

#if 0
{
	{
#endif
	}
}
