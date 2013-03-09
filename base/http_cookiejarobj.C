/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/cookiejar.H"
#include "x/strtok.H"
#include "x/chrcasecmp.H"
#include "x/uriimpl.H"
#include "x/http/responseimpl.H"
#include "x/http/cookie.H"
#include "http_storedcookie.H"
#include "http_domaincookies.H"
#include "x/property_value.H"
#include <algorithm>

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

static property::value<size_t>
maxcookiesprop(LIBCXX_NAMESPACE_WSTR "::http::cookiejar::max", 3000);

static property::value<size_t>
maxdomaincookiesprop(LIBCXX_NAMESPACE_WSTR "::http::cookiejar::domainmax", 50);

static property::value<size_t>
maxbytesprop(LIBCXX_NAMESPACE_WSTR "::http::cookiejar::cookiebytesmax", 4096);

cookiejarObj::cookiejarObj() : cookiedomainhier(cookiedomainhier_t::create())
{
}

cookiejarObj::~cookiejarObj() noexcept
{
}

void cookiejarObj::store(const uriimpl &request,
			 const responseimpl &resp)
{
	std::list<cookie> cookies;

	resp.getCookies(cookies);

	time_t now=time(NULL);

	for (auto &cookie:cookies)
	{
		if (cookie.name.size() + cookie.value.size()
		    >maxbytesprop.getValue())
			continue; // Too big of a cookie

		std::string path=cookie.getValidatedPath(request);
		std::string domain=cookie.getValidatedDomain(request);

		if (path.empty() || domain.empty())
			continue;

		cookie.path=path;
		cookie.domain=domain;

		auto newcookie=ref<storedcookieObj>::create(cookie);

		if (cookie.expiration != (time_t)-1 &&
		    cookie.expiration <= now)
		{
			// Setting expiration to a previous time removes
			// the cookie completely.

			cookiemrulist_t::lock allcookies_lock(allcookies);

			std::list<std::string> domain;
			domain_key(cookie.domain, domain);

			auto writer_lock=cookiedomainhier->create_writelock();

			if (writer_lock->to_child(domain, false))
			{
				// The cookie's domain exists.

				remove(allcookies_lock, writer_lock, newcookie);
			}
			return;
		}

		store(newcookie);
	}
}

void cookiejarObj::remove(cookiemrulist_t::lock &allcookies_lock,
			  const cookiedomainhier_t::base::writelock
			  &writer_lock,
			  const ref<storedcookieObj> &cookie)
{
	ref<domaincookiesObj> p=writer_lock->entry();

	p->remove(allcookies_lock, cookie);

	// If there are no more cookies in the domain,
	// remove the entire domain object.
	if (p->cookiepathhier->begin() == p->cookiepathhier->end())
		writer_lock->erase();
}

void cookiejarObj::store(const cookie &c)
{
	store(ref<storedcookieObj>::create(c));
}

void cookiejarObj::store(const ref<storedcookieObj> &cookie)
{
	cookiemrulist_t::lock allcookies_lock(allcookies);

	size_t maxcookies=maxcookiesprop.getValue();

	while (maxcookies && allcookies_lock->count >= maxcookies)
	{
		auto oldest_cookie=*--allcookies_lock->list.end();

		std::list<std::string> domain;
		domain_key(oldest_cookie->domain, domain);

		auto writer_lock=cookiedomainhier->create_writelock();

		if (!writer_lock->to_child(domain, false))
			throw EXCEPTION("Internal error, did not find the domain object");
		remove(allcookies_lock, writer_lock, oldest_cookie);
	}

	// Register the new cookie in allcookies. Unregister if an exception
	// gets thrown before this is finished.

	cookie->all_cookie=allcookies_lock->insert(cookie);

	try {
		// Compute hierarchy
		std::list<std::string> domain;

		domain_key(cookie->domain, domain);

		// This is blocked by the hier iterator created in begin(),
		// which uses a read lock.

		auto writer_lock=cookiedomainhier->create_writelock();

		writer_lock->
			insert([&]
			       {
				       // No such domain exists, create it.

				       auto p=ref<domaincookiesObj>::create();

				       p->store(allcookies_lock, cookie);
				       return p;
			       }, domain,
			       [&]
			       (ref<domaincookiesObj> &&p)
			       {
				       // The domain exists, store the cookie
				       // there, and return false so that the
				       // hier entry does not get replaced.

				       p->store(allcookies_lock, cookie);
				       return false;
			       });

	} catch (...) {
		allcookies_lock->remove(cookie->all_cookie);
		throw;
	}

}

void cookiejarObj::find(const uriimpl &uri,
			 std::list<std::pair<std::string,
			 std::string> > &cookies,
			 bool for_http)
{
	// Which cookies have been seen
	std::set<std::string> seen_names;

	std::list<std::string> domain, path;

	std::string uri_domain=uri.getHostPort().first;

	std::transform(uri_domain.begin(),
		       uri_domain.end(),
		       uri_domain.begin(),
		       chrcasecmp::tolower);

	domain_key(uri_domain, domain);

	if (domain.empty())
		return;

	std::string scheme=uri.getScheme();

	std::transform(scheme.begin(),
		       scheme.end(),
		       scheme.begin(),
		       chrcasecmp::tolower);

	strtok_str(uri.getPath(), "/", path);

	time_t now=time(NULL);

	auto lock=cookiedomainhier->search(domain);

	do
	{
		if (!lock->exists())
			continue;

		ref<domaincookiesObj> domaincookies=lock->entry();

		auto pathlock=domaincookies->cookiepathhier->search(path);

		do
		{
			if (!pathlock->exists())
				continue;

			for (const auto &cookie: pathlock->entry()->cookies)
			{
				// Does this cookie pass scrutiny?

				if (cookie->domain.substr(0, 1) != "." &&
				    cookie->domain != uri_domain)
					continue; // Origin server only

				if (!for_http && cookie->httponly)
					continue;

				if (cookie->secure && scheme != "https")
					continue;

				if (cookie->has_expiration() &&
				    cookie->expiration < now)
					continue;

				// We end up iterating longest domain and path
				// to shortest, and insert() does not replace
				// an existing entry, in a set.

				if (seen_names.insert(cookie->name).second)
				{
					cookies.push_back(std::make_pair
							  (cookie->name,
							   cookie->value));
				}

				// Refresh this cookie in the MRU list

				{
					cookiemrulist_t::lock lock(allcookies);

					cookie->all_cookie=
						lock->refresh(cookie->
							      all_cookie);
				}
			}
		} while (pathlock->to_parent());
	} while (lock->to_parent());
}

void cookiejarObj::domain_key(const std::string &domain,
			       std::list<std::string> &key)
{
	std::list<std::string> temp_key;

	strtok_str(domain, ".", temp_key);

	std::copy(temp_key.begin(), temp_key.end(),
		  std::front_insert_iterator<std::list<std::string> >(key));
}

//////////////////////////////////////////////////////////////////////////////

// Read lock that's acquired by domain iterators blocks create_writelock()
// in store().

class LIBCXX_HIDDEN cookiejarObj::iteratorimplObj : virtual public obj {

public:

	cookiedomainhier_t::base::iterator domain_beg_iter;
	cookiedomainhier_t::base::iterator domain_end_iter;

	domaincookiesObj::cookiepathhier_t::base::iterator path_beg_iter;
	domaincookiesObj::cookiepathhier_t::base::iterator path_end_iter;

	pathcookiesObj::cookies_t::iterator cookie_beg_iter;
	pathcookiesObj::cookies_t::iterator cookie_end_iter;

	iteratorimplObj(const cookiedomainhier_t::base::iterator
			&domain_beg_iterArg,
			const cookiedomainhier_t::base::iterator
			&domain_end_iterArg,
			const domaincookiesObj::cookiepathhier_t::base::iterator
			&path_beg_iterArg,
			const domaincookiesObj::cookiepathhier_t::base::iterator
			&path_end_iterArg,
			const pathcookiesObj::cookies_t::iterator
			&cookie_beg_iterArg,
			const pathcookiesObj::cookies_t::iterator
			&cookie_end_iterArg)
		: domain_beg_iter(domain_beg_iterArg),
		  domain_end_iter(domain_end_iterArg),
		path_beg_iter(path_beg_iterArg),
		path_end_iter(path_end_iterArg),
		cookie_beg_iter(cookie_beg_iterArg),
		cookie_end_iter(cookie_end_iterArg)
	{
	}

	iteratorimplObj(const iteratorimplObj &o)
		: domain_beg_iter(o.domain_beg_iter),
		  domain_end_iter(o.domain_end_iter),
		  path_beg_iter(o.path_beg_iter),
		  path_end_iter(o.path_end_iter),
		  cookie_beg_iter(o.cookie_beg_iter),
		  cookie_end_iter(o.cookie_end_iter)
	{
	}

	~iteratorimplObj() noexcept
	{
	}
};

cookiejarObj::iterator::iterator(const ptr<iteratorimplObj> &valueArg)
	: value(valueArg)
{
}

cookiejarObj::iterator::iterator()
{
}

cookiejarObj::iterator::iterator(const iterator &o)=default;

cookiejarObj::iterator::~iterator()
{
}

cookiejarObj::iterator &cookiejarObj::iterator::operator=(const iterator &o)
=default;

cookiejarObj::iterator &cookiejarObj::iterator::operator++()
{
	++value->cookie_beg_iter;

	nextnonempty();

	return *this;
}

void cookiejarObj::iterator::nextnonempty()
{
	bool newdomain=false;
	bool newpath=false;

	auto &impl=*value;

	// newdomain is flag indicating whether the domain iterator was just
	// advanced.
	//
	// newpath is a flag indicating whether the path iterator was just
	// advanced.

	while (1)
	{
		if (newdomain)
		{
			if (impl.domain_beg_iter == impl.domain_end_iter)
				break; // No more domains.

			auto domain=impl.domain_beg_iter.clone();

			// Advance to the first path in the domain

			newdomain=false;
			newpath=true;

			ref<domaincookiesObj> entry=domain->entry();

			impl.path_beg_iter=entry->cookiepathhier->begin();
			impl.path_end_iter=entry->cookiepathhier->end();
		}

		if (newpath)
		{
			// Just advanced the path iterators

			if (impl.path_beg_iter == impl.path_end_iter)
			{
				// No more paths, go to the next domain

				++impl.domain_beg_iter;
				newdomain=true;
				continue;
			}

			auto lock=impl.path_beg_iter.clone();

			newpath=false;

			ref<pathcookiesObj> path=lock->entry();
			impl.cookie_beg_iter=path->cookies.begin();
			impl.cookie_end_iter=path->cookies.end();
		}

		if (impl.cookie_beg_iter != impl.cookie_end_iter)
			return; // We have a cookie here.

		// Advance to the next path.

		++impl.path_beg_iter;
		newpath=true;
	}

	value=ptr<iteratorimplObj>();
}

const cookie *cookiejarObj::iterator::operator++(int)
{
	const cookie *p=&**value->cookie_beg_iter;

	operator++();

	return p;
}

cookie cookiejarObj::iterator::operator*() const
{
	return **value->cookie_beg_iter;
}

bool cookiejarObj::iterator::operator==(const iterator &o) const
{
	return value.null() && o.value.null();
}

cookiejarObj::iterator cookiejarObj::begin() const
{
	// Look for the first cookie. Start by iterating over the domains

	auto db=cookiedomainhier->begin(), de=cookiedomainhier->end();

	if (db != de)
	{
		// First domain.

		auto lock=db.clone();

		auto &entry=*lock->entry();

		// First path in the domain

		auto cb=entry.cookiepathhier->begin(),
			ce=entry.cookiepathhier->end();

		auto plock=cb.clone();

		auto &pentry=*plock->entry();

		auto impl=ref<iteratorimplObj>
			::create(db, de, cb, ce,
				 pentry.cookies.begin(),
				 pentry.cookies.end());

		iterator i(impl);

		i.nextnonempty(); // Shouldn't be necessary
		return i;
	}

	return end();
}

cookiejarObj::iterator cookiejarObj::end() const
{
	return iterator();
}

#if 0
{
	{
#endif
	}
}
