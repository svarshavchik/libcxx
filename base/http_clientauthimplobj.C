/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/clientauthimpl.H"
#include "x/chrcasecmp.H"
#include "http_auth_internal.H"
#include "x/base64.H"

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

clientauthimplObj::clientauthimplObj(auth schemeArg,
				     const std::string &realmArg)
	: scheme(schemeArg), realm(realmArg)
{
}

clientauthimplObj::~clientauthimplObj() noexcept
{
}

clientauthimplObj::basic::basic(const uriimpl &uriArg,
				const std::string &realm,
				const std::string &authstringArg)
	: clientauthimplObj(auth::basic, realm),
	  uri(uriArg), authstring(authstringArg)
{
	// Strip everything from the URI except the scheme and hostport.

	uriimpl::authority_t new_authority;

	new_authority.hostport=uri.getAuthority().hostport;
	uri.setAuthority(new_authority);
	uri.setPath("");
	uri.setQuery("");
	uri.setFragment("");
}

clientauthimplObj::basic::~basic() noexcept
{
}

void clientauthimplObj::basic::add_header(requestimpl &req, const char *header)
{
	req.append(header, authstring);
}

void clientauthimplObj::basic
::get_protection_space(const responseimpl &resp,
		       const clientauthcacheObj &cache,
		       const protection_space_t *&space,
		       std::list< std::list<std::string> > &hier)
{
	std::list<std::string> by_realm, by_uri;

	cache.get_default_protection_space(uri, resp, auth::basic,
					   realm, space, by_realm, by_uri);

	hier.push_back(by_realm);
	hier.push_back(by_uri);
}

clientauthimplptr clientauthimplObj::basic::new_request(const requestimpl &req)
{
	return clientauthimplptr(this);
}

clientauthimplptr
clientauthimplObj::basic::stale(const uriimpl &request_uri,
				const responseimpl::scheme_parameters_t &req)
{
	return clientauthimplptr();
}

void clientauthimplObj::basic
::authentication_info(const requestimpl &req,
		      const responseimpl::scheme_parameters_t &info)
{
}

clientauthimpl clientauthimplBase::create_basic(const uriimpl &uri,
						const std::string &realm,
						const std::string &userid,
						const std::string &password)
{
	return create_basic(uri, realm, userid + ":" + password);
}

clientauthimpl clientauthimplBase::create_basic(const uriimpl &uri,
						const std::string &realm,
						const std::string &up)
{

	std::ostringstream o;

	base64<>::encode(up.begin(), up.end(),
			 std::ostreambuf_iterator<char>(o.rdbuf()),
			 0);

	return ref<clientauthimplObj::basic>
		::create(uri, realm,
			 auth_tostring(auth::basic) + " " + o.str());
}

std::string auth_tostring(auth a)
{
	const char *p="unknown";

	switch (a) {
	case http::auth::basic:
		p="Basic";
		break;
	case http::auth::digest:
		p="Digest";
		break;
	}
	return p;
}


//! Convert authorization scheme from a string

auth auth_fromstring(const std::string &s)
{
	if (chrcasecmp::compare(s, auth_tostring(auth::basic)) == 0)
		return auth::basic;
	if (chrcasecmp::compare(s, auth_tostring(auth::digest)) == 0)
		return auth::digest;

	return auth::unknown;
}

#if 0
{
	{
#endif
	}

}
