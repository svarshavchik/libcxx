/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/opendirobj.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

opendirObj::opendirObj() noexcept : dirp(0)
{
}

opendirObj::~opendirObj()
{
	if (dirp)
		closedir(dirp);
}

#if 0
{
#endif
}
