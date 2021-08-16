/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef http_clientauthimplobj_H
#define http_clientauthimplobj_H

#include <x/http/requestimpl.H>
#include <x/http/responseimpl.H>
#include <x/http/clientauthimplfwd.H>
#include <x/hier.H>
#include <x/obj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
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
			  const std::string &realmArg);

	//! Destructor
	~clientauthimplObj();

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

	//! Add this authenticator's headers to a request, in advance

	//! Returns a non-null reference if this authenticator should be
	//! added to the headers of the initial request. Cached basic
	//! authenticator returns itself. Cached digest authenticator
	//! constructs a reference to a clientauthimpl whose add_header(),
	//! if necessary, inserts the headers with an updated cnonce header.
	//!
	//! A null reference pointer gets returned in the event that this
	//! authenticator should not be added to the initial request.

	virtual clientauthimplptr new_request(//! The new request
					      const requestimpl &req)
		LIBCXX_HIDDEN=0;

	//! Whether to try again in response to a server's challenge.

	//! If a null reference gets returned, the existing authenticator
	//! is considered to have failed, and it gets removed from
	//! the clientauthcache.
	//!
	//! If a reference gets returned, it's considered to be a replacement
	//! authenticator for the response to the server's challenge.
	//!
	//! Digest authenticator returns a non-null reference if the
	//! server's challenge has the stale flag set. Otherwise, and in every
	//! case for the basic authenticator, a null reference pointer gets
	//! return.

	virtual clientauthimplptr stale(//! The URI that sent the challenge
					const uriimpl &request_uri,

					//! The challenge's parameters.
					const responseimpl::scheme_parameters_t
					&req)
		LIBCXX_HIDDEN=0;

	//! Process the received authentication-info header

	virtual void authentication_info(//! The original request
					 const requestimpl &req,

					 //! The authentication-info header in the response
					 const responseimpl::scheme_parameters_t
					 &info)
		LIBCXX_HIDDEN=0;

	class basic;
	class digest;
};

#if 0
{
#endif
}
#endif
