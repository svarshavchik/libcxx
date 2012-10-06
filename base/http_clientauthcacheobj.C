/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/requestimpl.H"
#include "x/http/clientauth.H"
#include "x/http/clientauthimpl.H"
#include "x/strtok.H"
#include "x/chrcasecmp.H"
#include "x/join.H"
#include "x/tostring.H"
#include "x/messages.H"
#include "http_auth_internal.H"
#include "gettext_in.h"
#include <algorithm>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::http::clientauthcacheObj);

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

clientauthcacheObj::clientauthcacheObj()
	: proxy_authorizations(protection_space_t::create()),
	  www_authorizations(protection_space_t::create())
{
}

clientauthcacheObj::~clientauthcacheObj() noexcept
{
}

std::string clientauthcacheObj::decompose_authority(const uriimpl &uri)
{
	std::ostringstream o;

	auto hostport=uri.getHostPort();

	o << uri.getScheme() << ":" << hostport.first
	  << ":" << hostport.second;

	std::string n=o.str();

	std::transform(n.begin(), n.end(), n.begin(), chrcasecmp::tolower);

	return n;
}

void clientauthcacheObj::decompose_path(const uriimpl &uri,
					std::list<std::string> &path)
{
	// Note that strtok_str() strips off the trailing /.

	strtok_str(uri.getPath(), "/", path);
}

void clientauthcacheObj
::save_user_password_authorization(const responseimpl &resp,
				   const uriimpl &uri,
				   auth scheme,
				   const std::string &realm,
				   const responseimpl
				   ::scheme_parameters_t &params,
				   const std::string &userid,
				   const std::string &password)
{
	switch (scheme) {
	case auth::basic:
		save_authorization(resp, uri, clientauthimpl::base
				   ::create_basic(uri, realm, userid,
						  password));
		return;
	case auth::digest:
		{
			// Weak symbol, must link with libcxxtls to get it.

			auto impl=&clientauthimpl::base::create_digest;

			if (!impl)
			{
				LOG_WARNING(libmsg(_txt("libcxxtls required for digest authentication")));
				return;
			}

			auto auth=(*impl)(uri, realm, params, userid, password);
			if (!auth.null())
				save_authorization(resp, uri, auth);
		}
	}
}

void clientauthcacheObj::save_basic_authorization(const responseimpl &resp,
						  const uriimpl &uri,
						  const std::string &realm,
						  const std::string &up)
{
	save_authorization(resp, uri, clientauthimpl::base
			   ::create_basic(uri, realm, up));
}

void clientauthcacheObj::save_authorization(const responseimpl &resp,
					    const uriimpl &domain,
					    const clientauthimpl &auth)
{
	const protection_space_t *space;

	std::list<std::list<std::string> > hier;

	auth->get_protection_space(resp, *this, space, hier);

	if (!space)
		return;

	LOG_DEBUG("Updating authorization for scheme "
		  << auth_tostring(auth->scheme)
		  << ", realm " << auth->realm
		  << ", code " << resp.getStatusCode());

	auto writelock=(*space)->create_writelock();

	for (const std::list<std::string> &h:hier)
	{
		LOG_TRACE("Authorization space: " << join(h, "/"));
		// Return write lock to the root node.

		while (writelock->to_parent())
			;

		writelock->insert([&auth] { return auth; },
				  h,
				  [&auth]
				  (clientauthimpl && old_auth)
				  {
					  // Replace only less preferred
					  if (old_auth->scheme
					      < auth->scheme)
						  return true;

					  LOG_TRACE("Ignored, kept existing"
						    " scheme: "
						    << auth_tostring(old_auth->
								     scheme));
					  return false;
				  });
	}
}

void clientauthcacheObj::search_authorizations(const requestimpl &req,
					       const clientauth &auth)
{
	LOG_DEBUG("Searching proxy authorizations, uri: "
		  << tostring(req.getURI()));

	{
		auto lock=proxy_authorizations->create_readlock();

		if (lock->exists())
		{
			auto entry=lock->entry();

			auto new_entry=entry->new_request(req);

			if (new_entry.null())
			{
				LOG_TRACE("Skipped proxy authorization for "
					  << entry->realm);
			}
			else
			{
				LOG_TRACE("Added proxy authorization for "
					  << new_entry->realm);


				auth->proxy_authorizations
					.insert(std::make_pair(new_entry->realm,
							       new_entry));
			}
		}
	}

	// Pick the default proxy authorization, if it exists

	std::list<std::string> path;

	// Search www authorizations in [rootnode]/[authority]/[realm]
	// parent.

	path.push_back(""); // Placeholder for [realm].

	decompose_path(req.getURI(), path);

	const std::string authority=decompose_authority(req.getURI());

	auto lock=www_authorizations->create_readlock();

	if (!lock->to_child(authority))
		return; // Nothing at all for this authority.

	std::set<std::string> realms=lock->children();

	for (const std::string &realm:realms)
	{
		// Reposition to where we were.

		if (!lock->to_parent())
			;
		if (!lock->to_child(authority))
			return; // Shouldn't happen, of course.

		path.front()=realm;

		if (!lock->to_child(path, true))
			continue;

		auto entry=lock->entry();

		auto new_entry=entry->new_request(req);

		if (new_entry.null())
		{
			LOG_TRACE("Skipped proxy authorization for "
				  << join(lock->name(), "/")
				  << ", realm: "
				  << entry->realm);
		}
		else
		{
			auth->www_authorizations
				.insert(std::make_pair(new_entry->realm,
						       new_entry));

			LOG_TRACE("Added www authorization for "
				  << join(lock->name(), "/")
				  << ", realm: "
				  << entry->realm);
		}
	}
}

bool clientauthcacheObj
::fail_authorization(const uriimpl &uri,
		     const responseimpl &resp,
		     const clientauth &authorizations,
		     auth scheme,
		     const std::string &realm,
		     const responseimpl::scheme_parameters_t &params)
{
	LOG_DEBUG("Authorization failed: status code "
		  << resp.getStatusCode()
		  << ", scheme " << auth_tostring(scheme)
		  << ", realm " << realm);

	do_fail_authorization(uri, resp, authorizations,
			      auth::basic, "", params);
	bool rc=do_fail_authorization(uri, resp, authorizations,
				      scheme, realm, params);

	LOG_DEBUG("Cached authorization: " << (rc ? "found":"not found"));
	return rc;
}

bool clientauthcacheObj
::do_fail_authorization(const uriimpl &uri,
			const responseimpl &resp,
			const clientauth &authorizations,
			auth scheme,
			const std::string &realm,
			const responseimpl::scheme_parameters_t &params)
{
	if (resp.proxy_authentication_required())
	{
		return fail_authorization(uri, resp, authorizations,
					  authorizations->proxy_authorizations,
					  scheme,
					  realm,
					  params);
	}

	if (resp.www_authentication_required())
	{
		return fail_authorization(uri, resp, authorizations,
					  authorizations->www_authorizations,
					  scheme,
					  realm,
					  params);
	}
	return false;
}

bool clientauthcacheObj
::fail_authorization(const uriimpl &uri,
		     const responseimpl &resp,
		     const clientauth &authorizations,
		     std::map<std::string, clientauthimpl> &authorization_map,
		     auth scheme,
		     const std::string &realm,
		     const responseimpl::scheme_parameters_t &params)
{
	LOG_TRACE("Searching authorization space for realm \""
		  << realm << "\"");

	{
		auto scheme=authorization_map.find(realm);

		if (scheme != authorization_map.end())
		{
			const protection_space_t *space;
			std::list<std::list<std::string> > hier;

			auto updated=scheme->second->stale(uri, params);

			if (!updated.null())
			{
				scheme->second=updated;

				LOG_TRACE("Updated stale authorization for"
					  " realm \""
					  << scheme->second->realm << "\"");
				return true;
			}

			LOG_TRACE("Removing failed authorization for realm \""
				  << scheme->second->realm << "\"");

			scheme->second->get_protection_space(resp, *this,
							     space, hier);

			if (space)
			{
				auto writelock=(*space)->create_writelock();

				for (const std::list<std::string> &h:hier)
				{
					LOG_TRACE("Authorization space: "
						  << join(h, "/"));

					while (writelock->to_parent())
						;

					if (!writelock->to_child(h, true))
						continue;

					auto entry=writelock->entry();

					if (entry->realm == scheme->second
					    ->realm)
					{
						LOG_TRACE("Removed cached"
							  " authorization for"
							  " scheme \""
							  << auth_tostring
							  (entry->scheme)
							  << "\"");

						writelock->erase();
					}
				}
			}
			authorization_map.erase(scheme);
			return false;
		}
	}

	if (realm.empty())
		return false;
	// We are here only to remove the default basic realm from the cache,
	// that failed.

	LOG_TRACE("Retrieving default protection space for realm \""
		  << realm << "\" in "
		  << tostring(uri));

	const protection_space_t *space;
	std::list<std::string> realm_hier;

	get_default_protection_space(uri, resp, realm, space, realm_hier);

	if (!space)
		return false;

	LOG_TRACE("Searching authorization space: " << join(realm_hier, "/"));

	auto readlock=(*space)->search(realm_hier);

	if (readlock->exists())
	{
		auto entry=readlock->entry();

		if (entry->scheme == scheme)
		{
			authorization_map.insert(std::make_pair(entry->realm,
								entry));
			return true;
		}
	}
	return false;
}

void clientauthcacheObj
::get_default_protection_space(const uriimpl &uri,
			       const responseimpl &resp,
			       const std::string &realm,
			       const protection_space_t *&space,
			       std::list<std::string> &realm_hier) const
{
	space=nullptr;

	realm_hier.clear();

	if (resp.proxy_authentication_required())
	{
		space=&proxy_authorizations;
	}
	else if (resp.www_authentication_required())
	{
		// For www authorizations, we install in
		// [rootnode]/[authority]/[realm].

		space=&www_authorizations;

		realm_hier.push_back(clientauthcacheObj
				     ::decompose_authority(uri));
	}
	else return;

	// proxy: {rootnote]/realm
	// www: [rootnode]/authority/realm
	realm_hier.push_back(realm);
}

#if 0
{
	{
#endif
	}
}
