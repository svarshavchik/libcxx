/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/requestimpl.H"
#include "x/http/clientauth.H"
#include "http_auth_internal.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::http::clientauthObj);

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

clientauthObj::clientauthObj()
{
}

clientauthObj::~clientauthObj()
{
}

void clientauthObj::add_headers(requestimpl &req)
{
	// Remove any old headers

	req.erase(req.proxy_authorization);
	req.erase(req.www_authorization);

	// Now, add the new headers
	for (const auto &auth:proxy_authorizations)
	{
		LOG_DEBUG("Adding proxy authorization headers for realm "
			  << auth.first
			  << ", scheme " << auth_tostring(auth.second->scheme));
		auth.second->add_header(req, req.proxy_authorization);
	}

	for (const auto &auth:www_authorizations)
	{
		LOG_DEBUG("Adding www authorization headers for realm "
			  << auth.first
			  << ", scheme " << auth_tostring(auth.second->scheme));
		auth.second->add_header(req, req.www_authorization);
	}
}

#if 0
{
	{
#endif
	}
}
