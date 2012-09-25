/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef http_clientauthimplobj_H
#define http_clientauthimplobj_H

#include <x/http/requestimpl.H>
#include <x/http/clientauthimplfwd.H>
#include <x/hier.H>
#include <x/obj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	}
};
#endif

class clientauthcacheObj;

//! Superclass for HTTP client-side authenticators

//! \internal
//!
class clientauthimplObj : virtual public obj {

public:

	//! The scheme implemented by this authenticator

	const auth scheme;

	//! The authenticator's realm

	const std::string realm;

	//! Constructor
	clientauthimplObj(auth schemeArg,
			  const std::string &realmArg) LIBCXX_HIDDEN;

	//! Destructor
	~clientauthimplObj() noexcept LIBCXX_HIDDEN;

	//! Add authorization headers
	virtual void add_header(//! Request to add headers to
				requestimpl &req,

				//! www_authorization or proxy_authorization
				const char *header) LIBCXX_HIDDEN =0;

	//! An internal container for the protection space hierarchy
	typedef hier<std::string, clientauthimpl> protection_space_t;

	//! A nullable pointer to a protection space hierarchy
	typedef hierptr<std::string, clientauthimpl> protection_spaceptr_t;

	//! Enumerate the protection space this authorization scheme covers

	//! This is an internal list of hierarchical keys that cache
	//! authorization objects. Each key is a std::list<std::string>,
	//! and an authorization module may claim authorization over multiple
	//! protection spaces.
	//!
	//! The authorization module initializes space, and hier, which is
	//! a list of protection spaces.

	virtual void get_protection_space(//! HTTP response with a challenge
					  const responseimpl &resp,

					  //! An internal cache for authorizations
					  const clientauthcacheObj &cache,

					  //! Protection space from the cache

					  const protection_space_t *&space,

					  //! Protection space hierarchies.

					  std::list< std::list<std::string>
					  > &hier) LIBCXX_HIDDEN=0;

	class basic;
};

#if 0
{
	{
#endif
	}
}
#endif
