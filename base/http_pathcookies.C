/*
** Copyright 2012 Double Precision, Inc.
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

pathcookiesObj::~pathcookiesObj() noexcept
{
}

bool pathcookiesObj::cmp::operator()(const ref<storedcookieObj> &a,
				     const ref<storedcookieObj> &b)
{
	return a->name < b->name;
}

#if 0
{
	{
#endif
	}
}