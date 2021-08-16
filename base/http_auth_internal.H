/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef http_auth_internal_H
#define http_auth_internal_H

#include "x/http/clientauth.H"
#include "x/namespace.h"

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

//! Basic authentication scheme implementation.

class LIBCXX_HIDDEN clientauthimplObj::basic : public clientauthimplObj {

 public:

	//! The uri. Just the scheme+authority.

	uriimpl uri;

	//! Precomputed authentication string, it's always the same.

	std::string authstring;

	//! Constructor
	basic(const uriimpl &uriArg,
	      const std::string &realm, const std::string &authstringArg);
	~basic();

	//! Add my header to a request
	void add_header(requestimpl &req, const char *header) override;

	//! Return my protection space.
	void get_protection_space(const responseimpl &resp,
				  const clientauthcacheObj &cache,
				  const protection_space_t *&space,
				  std::list< std::list<std::string> > &hier)
		override;


	//! Calculate basic protection space for a URI

	//! This is invoked by the basic scheme's authenticator's
	//! get_protection_space(), and also by fail_authorizations(), which
	//! uses the same algorithm.
	//!
	//! The static method returns an extra flag for each hierarchy,
	//! whether fail_authorization() should prune any inferior nodes,
	//! an addition to removing the indicated one. This is set for the
	//! realm-specific entry, and cleared for the authority entry.

	static void get_protection_space(const uriimpl &uri,
					 const std::string &realm,
					 const responseimpl &resp,
					 const clientauthcacheObj &cache,
					 const protection_space_t *&space,
					 std::list< std::list<std::string> >
					 &hier);

	//! Add this authenticator to a new request.

	clientauthimplptr new_request(const requestimpl &req) override;

	//! There's no such thing as a stale challenge, this request failed.

	clientauthimplptr stale(const uriimpl &request_uri,
				const responseimpl::scheme_parameters_t &req)
		override;

	//! Ignored, not applicable

	void authentication_info(const requestimpl &req,
				 const responseimpl::scheme_parameters_t &info)
		override;
};

#if 0
{
#endif
}
#endif
