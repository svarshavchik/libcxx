/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_clientauthobj_H
#define x_http_clientauthobj_H

#include <x/obj.H>
#include <x/uriimpl.H>
#include <x/http/requestimpl.H>
#include <x/http/clientauthfwd.H>
#include <x/http/clientauthimplfwd.H>
#include <x/logger.H>
#include <string>
#include <map>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

//! Collected authorizations for an HTTP request.

class clientauthObj : virtual public obj {

	//! The logger
	LOG_CLASS_SCOPE;
public:
	//! The internal map of proxy authorizations, by realm.
	std::map<std::string, clientauthimpl > proxy_authorizations;

	//! The internal map of www authorizations, by realm.
	std::map<std::string, clientauthimpl > www_authorizations;

	//! Constructor
	clientauthObj();

	//! Destructor
	~clientauthObj();

	//! Add the authorization headers to a request.

	void add_headers(requestimpl &req);
};

#if 0
{
#endif
}

#endif
