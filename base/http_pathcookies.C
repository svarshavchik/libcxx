/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "http_pathcookies.H"

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

pathcookiesObj::pathcookiesObj()
{
}

pathcookiesObj::~pathcookiesObj()
{
}

bool pathcookiesObj::cmp::operator()(const ref<storedcookieObj> &a,
				     const ref<storedcookieObj> &b)
	const noexcept
{
	return a->name < b->name;
}

#if 0
{
	{
#endif
	}
}
