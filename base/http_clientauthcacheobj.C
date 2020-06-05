/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/requestimpl.H"
#include "x/http/clientauth.H"
#include "x/http/clientauthimpl.H"
#include "x/strtok.H"
#include "x/chrcasecmp.H"
#include "x/join.H"
#include "x/to_string.H"
#include "x/messages.H"
#include "http_auth_internal.H"
#include "gettext_in.h"
#include <algorithm>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::http::clientauthcacheObj);

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

clientauthcacheObj::clientauthcacheObj()
	: proxy_authorizations(protection_space_t::create()),
	  www_authorizations(protection_space_t::create())
{
}

clientauthcacheObj::~clientauthcacheObj()
{
}

std::string clientauthcacheObj::decompose_authority(const uriimpl &uri)
{
	auto hostport=uri.get_host_port();

	std::string n=uri.get_scheme() + ":" + hostport.first
	  + ":" + to_string(hostport.second, locale::base::c());

	std::transform(n.begin(), n.end(), n.begin(), chrcasecmp::tolower);

	return n;
}

void clientauthcacheObj::decompose_path(const uriimpl &uri,
					std::list<std::string> &path)
{
	// Note that strtok_str() strips off the trailing /.

	strtok_str(uri.get_path(), "/", path);
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
		  << ", code " << resp.get_status_code());

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
		  << to_string(req.get_URI()));

	const std::string authority=decompose_authority(req.get_URI());

	{
		auto lock=proxy_authorizations->create_readlock();

		std::list<std::string> path;

		path.push_back("uri");
		path.push_back(authority);

		search_authorization_path(req, lock,
					  auth->proxy_authorizations, path);
	}

	LOG_DEBUG("Searching www authorizations, uri: "
		  << to_string(req.get_URI()));

	auto lock=www_authorizations->create_readlock();

	std::list<std::string> path;

	path.push_back("uri");
	path.push_back(authority);

	search_authorization_path(req, lock,
				  auth->www_authorizations, path);
}

void clientauthcacheObj
::search_authorization_path(const requestimpl &req,
			    const protection_space_t::base::readlock &lock,
			    authorization_map_t &auth_map,
			    std::list<std::string> &path)
{
	decompose_path(req.get_URI(), path);

	if (!lock->to_child(path, true))
	{
		LOG_TRACE("Search path " << join(path, "/") << " not found");
		return;
	}

	auto entry=lock->entry();

	auto new_entry=entry->new_request(req);

	if (new_entry.null())
	{
		LOG_TRACE("Skipped authorization for "
			  << join(lock->name(), "/")
			  << ", realm: "
			  << entry->realm);
	}
	else
	{
		auth_map.emplace(new_entry->realm, new_entry);

		LOG_TRACE("Added authorization for "
			  << join(lock->name(), "/")
			  << ", realm: "
			  << entry->realm);
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
		  << resp.get_status_code()
		  << ", scheme " << auth_tostring(scheme)
		  << ", realm " << realm);

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
		return fail_authorization(uri, resp,
					  authorizations->proxy_authorizations,
					  scheme,
					  realm,
					  params);
	}

	if (resp.www_authentication_required())
	{
		return fail_authorization(uri, resp,
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
		     authorization_map_t &authorization_map,
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
		  << to_string(uri));

	const protection_space_t *space;
	std::list<std::string> realm_hier;
	std::list<std::string> uri_hier;

	get_default_protection_space(uri, resp, scheme,
				     realm, space, realm_hier, uri_hier);

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
			       auth schema,
			       const std::string &realm,
			       const protection_space_t *&space,
			       std::list<std::string> &by_realm,
			       std::list<std::string> &by_uri) const
{
	space=nullptr;

	by_realm.clear();
	by_uri.clear();

	if (resp.proxy_authentication_required())
	{
		space=&proxy_authorizations;

		by_realm.push_back("realm");
		by_realm.push_back(realm);

		by_uri.push_back("uri");
		if (schema != auth::basic)
		{
			by_uri.push_back(decompose_authority(uri));
			decompose_path(uri, by_uri);
		}
	}

	if (resp.www_authentication_required())
	{
		std::string authority=decompose_authority(uri);

		// For www authorizations, we install in
		// [rootnode]/[authority]/[realm].

		space=&www_authorizations;

		by_realm.push_back("realm");
		by_realm.push_back(authority);
		by_realm.push_back(realm);

		by_uri.push_back("uri");
		by_uri.push_back(authority);

		if (schema != auth::basic)
			decompose_path(uri, by_uri);
	}
}

#if 0
{
#endif
}
