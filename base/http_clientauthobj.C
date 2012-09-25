/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/requestimpl.H"
#include "x/http/clientauth.H"
#include "http_auth_internal.H"

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

clientauthObj::clientauthObj()
{
}

clientauthObj::~clientauthObj() noexcept
{
}

void clientauthObj::add_headers(requestimpl &req)
{
	// Remove any old headers

	req.erase(req.proxy_authorization);
	req.erase(req.www_authorization);

	// Now, add the new headers
	for (const auto &auth:proxy_authorizations)
		auth.second->add_header(req, req.proxy_authorization);

	for (const auto &auth:www_authorizations)
		auth.second->add_header(req, req.www_authorization);
}

#if 0
{
	{
#endif
	}
}
