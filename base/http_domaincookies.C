/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "http_domaincookies.H"
#include "http_storedcookie.H"
#include "x/strtok.H"

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif


domaincookiesObj::domaincookiesObj()
	: cookiepathhier(cookiepathhier_t::create())
{
}

domaincookiesObj::~domaincookiesObj() noexcept
{
}

void domaincookiesObj::store(cookiemrulist_t::lock &all_lock,
			     const ref<storedcookieObj> &cookie,
			     size_t maxdomaincookies)
{
	cookiemrulist_t::lock domaincookies_lock(allcookies);

	// Prune the list of cookies

	while (maxdomaincookies &&
	       domaincookies_lock->count >= maxdomaincookies)
	{
		remove(all_lock, domaincookies_lock,
		       *--domaincookies_lock->list.end());
	}

	// Register the new cookie in allcookies. Unregister if an exception
	// gets thrown before this is finished.

	cookie->domain_cookie=domaincookies_lock->insert(cookie);

	try {
		// Compute hierarchy
		std::list<std::string> path;
		strtok_str(cookie->path, "/", path);

		auto writer_lock=cookiepathhier->create_writelock();

		writer_lock->
			insert([&]
			       {
				       // No such path exists, create it.

				       auto p=ref<pathcookiesObj>::create();

				       cookie->path_owner=&*p;

				       p->cookies.insert(cookie);

				       return p;
			       }, path,
			       [&]
			       (ref<pathcookiesObj> &&p)
			       {
				       cookie->path_owner=&*p;

				       // Ok, there are cookies at the
				       // same path. If this cookie
				       // already exists, erase it.

				       auto old=p->cookies.find(cookie);

				       if (old != p->cookies.end())
				       {
					       drop(all_lock,
						    domaincookies_lock, p,
						    old);
				       }
				       p->cookies.insert(cookie);
				       return false;
			       });

	} catch (...) {
		domaincookies_lock->remove(cookie->domain_cookie);
		throw;
	}
}

void domaincookiesObj::remove(cookiemrulist_t::lock &all_lock,
			      const ref<storedcookieObj> &cookie)
{
	cookiemrulist_t::lock domaincookies_lock(allcookies);

	remove(all_lock, domaincookies_lock, cookie);
}

void domaincookiesObj::remove(cookiemrulist_t::lock &all_lock,
			      cookiemrulist_t::lock &domaincookies_lock,
			      const ref<storedcookieObj> &cookie)
{
	std::list<std::string> path;
	strtok_str(cookie->path, "/", path);

	auto writer_lock=cookiepathhier->create_writelock();

	if (!writer_lock->to_child(path, false))
		return; // Does not exist.

	ref<pathcookiesObj> p=writer_lock->entry();

	auto old=p->cookies.find(cookie);

	if (old == p->cookies.end())
		return;

	drop(all_lock, domaincookies_lock, p, old);

	// If no more cookies at this path, remove the path object entirely.

	if (p->cookies.empty())
		writer_lock->erase();
}

void domaincookiesObj::drop(cookiemrulist_t::lock &all_lock,
			    cookiemrulist_t::lock &domaincookies_lock,
			    const ref<storedcookieObj> &oldest_cookie)
{
	ref<pathcookiesObj> p(oldest_cookie->path_owner);

	drop(all_lock, domaincookies_lock,
	     p,
	     p->cookies.find(oldest_cookie));
}

void domaincookiesObj::drop(cookiemrulist_t::lock &all_lock,
			    cookiemrulist_t::lock &domaincookies_lock,
			    const ref<pathcookiesObj> &p,
			    pathcookiesObj::cookies_t::iterator old)
{
	all_lock->remove((*old)->all_cookie);
	domaincookies_lock->remove((*old)->domain_cookie);
	p->cookies.erase(old);
}

#if 0
{
	{
#endif
	}
}
