/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/cookiejar.H"
#include "http_storedcookie.H"

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

cookiemrulist::cookiemrulist() : count(0)
{
}

cookiemrulist::~cookiemrulist() noexcept
{
}

std::list< ref<storedcookieObj> >::iterator
cookiemrulist::insert(const ref<storedcookieObj> &cookie)
{
	list.push_front(cookie);
	++count;
	return list.begin();
}

void cookiemrulist::remove(std::list< ref<storedcookieObj> >::iterator iter)
	noexcept
{
	list.erase(iter);
	--count;
}

std::list< ref<storedcookieObj> >::iterator
cookiemrulist::refresh(std::list< ref<storedcookieObj> >::iterator iter)
{
	auto newIter=insert(*iter);

	remove(iter);

	return newIter;
}

#if 0
{
	{
#endif
	}
}
