/*
** Copyright 2012-2019 Double Precision, Inc.
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

cookiemrulist::ps_mrulist::ps_mrulist() : count(0)
{
}

cookiemrulist::cookiemrulist()
{
}

cookiemrulist::~cookiemrulist()
{
}

cookiemrulist::cookie_mru_iter_t
cookiemrulist::insert(const ref<storedcookieObj> &cookie)
{
	std::string domain=cookie->domain;

	if (domain.substr(0, 1) == ".")
		domain=domain.substr(1);

	std::string public_suffix=
		cookiejar::base::public_suffix(domain);

	auto iter=mru.find(public_suffix);

	if (iter == mru.end())
		iter=mru.insert(std::make_pair(public_suffix, ps_mrulist()))
			.first;

	iter->second.list.push_front(cookie);
	++iter->second.count;
	return std::make_pair(iter, iter->second.list.begin());
}

void cookiemrulist::remove(cookiemrulist::cookie_mru_iter_t iter)
	noexcept
{
	iter.first->second.list.erase(iter.second);
	if (--iter.first->second.count == 0)
		mru.erase(iter.first);
}

cookiemrulist::cookie_mru_iter_t
cookiemrulist::refresh(cookiemrulist::cookie_mru_iter_t iter)
{
	iter.first->second.list.push_front(*iter.second);
	iter.first->second.list.erase(iter.second);

	return std::make_pair(iter.first, iter.first->second.list.begin());
}

#if 0
{
	{
#endif
	}
}
