/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "http_domaincookies.H"
#include "http_storedcookie.H"
#include "x/strtok.H"

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif


domaincookiesObj::domaincookiesObj()
	: cookiepathhier(cookiepathhier_t::create())
{
}

domaincookiesObj::~domaincookiesObj()
{
}

void domaincookiesObj::store(cookiemrulist_t::lock &all_lock,
			     const ref<storedcookieObj> &cookie)
{
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
				       drop(all_lock, p,
					    old);
			       }
			       p->cookies.insert(cookie);
			       return false;
		       });
}

void domaincookiesObj::remove(cookiemrulist_t::lock &all_lock,
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

	drop(all_lock, p, old);

	// If no more cookies at this path, remove the path object entirely.

	if (p->cookies.empty())
		writer_lock->erase();
}

void domaincookiesObj::drop(cookiemrulist_t::lock &all_lock,
			    const ref<storedcookieObj> &oldest_cookie)
{
	ref<pathcookiesObj> p(oldest_cookie->path_owner);

	drop(all_lock,
	     p,
	     p->cookies.find(oldest_cookie));
}

void domaincookiesObj::drop(cookiemrulist_t::lock &all_lock,
			    const ref<pathcookiesObj> &p,
			    pathcookiesObj::cookies_t::iterator old)
{
	all_lock->remove((*old)->all_cookie);
	p->cookies.erase(old);
}

#if 0
{
#endif
}
