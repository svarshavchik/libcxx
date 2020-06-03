/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef http_clientauthimpl_H
#define http_clientauthimpl_H

#include <x/http/clientauthimplfwd.H>
#include <x/http/clientauthimplobj.H>
#include <x/gcrypt/md.H>
#include <x/uriimpl.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

// ----------------------------------------------------------------------------

//! INSERT_LIBX_NAMESPACE::%http::%clientauthimpl::%base

//! \internal

class clientauthimplBase {

public:

	//! Create an implementation of a Basic authorization scheme.

	//! \internal
	//!
	static clientauthimpl create_basic(//! URI being authorized
					   const uriimpl &uri,

					   //! Basic realm
					   const std::string &realm,

					   //! Authorized userid
					   const std::string &userid,

					   //! Authorized password
					   const std::string &password)
		LIBCXX_HIDDEN;

	//! Create an implementation of a Basic authorization scheme.

	//! \internal
	//!
	static clientauthimpl create_basic(//! URI being authorized
					   const uriimpl &uri,

					   //! Basic realm
					   const std::string &realm,

					   //! Preformatted "userid:password" string.
					   const std::string &useridcolonpassword)
		LIBCXX_HIDDEN;
};

#if 0
{
#endif
}
#endif
