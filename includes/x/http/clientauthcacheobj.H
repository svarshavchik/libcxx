/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_clientauthcacheobj_H
#define x_http_clientauthcacheobj_H

#include <x/uriimpl.H>
#include <x/http/responseimpl.H>
#include <x/http/clientauthfwd.H>
#include <x/http/clientauthimplobj.H>
#include <x/gcrypt/md.H>
#include <x/logger.H>
#include <string>
#include <map>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	}
};
#endif

//! A hierarchy of known client authorizations

class clientauthcacheObj : virtual public obj {

	//! The logger object.
	LOG_CLASS_SCOPE;
public:

	//! typedef alias

	typedef clientauthimplObj::protection_space_t protection_space_t;

	//! typedef alias

	typedef clientauthimplObj::protection_spaceptr_t protection_spaceptr_t;

	//! Constructor
	clientauthcacheObj();

	//! Destructor
	~clientauthcacheObj();

	//! Install a manually-specific basic authorization scheme.

	void save_basic_authorization(//! Response for which authorization is provided
				      const responseimpl &resp,

				      //! Original request's URI
				      const uriimpl &uri,

				      //! Realm
				      const std::string &realm,

				      //! "userid:password" string
				      const std::string &useridcolonpassword);


	//! Save an authorization

	void save_authorization(//! Original response with a challenge,
				const responseimpl &resp,

				//! The domain for the authorization
				const uriimpl &domain,

				//! The authorization
				const clientauthimpl &auth);

	//! Search for any existing authorizations whose scope includes this URI

	//! This is done before sending the initial request, in order to save
	//! a round trip if we know what authorizations are known, in advance.
	//!
	//! We search for uri[authority]/[path] which corresponds with what
	//! get_default_protection_space() does.

	void search_authorizations(//! The request about to be made.

				   //! Authorization headers get added to this
				   //! request.
				   const requestimpl &req,

				   //! The authorizations get added here
				   const clientauth &auth);

	//! Received an authentication challenge.

	//! If we did not supply the authorization for the realm, and it exists
	//! in the cache, add it to the clientauth, and return true.
	//!
	//! Otherwise return false. If the authorization is cached, it must be
	//! no longer valid, so it gets removed also.

	bool fail_authorization(//! Original request URI
				const uriimpl &uri,

				//! Received response
				const responseimpl &resp,

				//! The authorizations that were in the request
				const clientauth &authorizations,

				//! Challenge scheme
				auth scheme,

				//! Challenge realm
				const std::string &realm,

				//! Challenge parameters
				const responseimpl::scheme_parameters_t
				&params);

	friend class clientauthimplObj;

 protected:
	//! Cached proxy authorizations
	protection_space_t proxy_authorizations;

	//! Cached www authorizations
	protection_space_t www_authorizations;

	//! Convert the authority part of a URI to a hierarchy node.

	//! Returns a single string: "scheme:host:port", from the URI,
	//! converted to lowercase. This is the node for the authority,
	//! in the authorization cache hierarchy.
	//!
	//! \internal

	static std::string decompose_authority(const uriimpl &uri);

	//! Decompose the URI's path into a hierarchical key.

	//! Split the path component of the URI by slashes, into separate
	//! words.
	//!
	//! \internal

	static void decompose_path(const uriimpl &uri,
				   std::list<std::string> &path);

	//! Calculate protection spaces for a URI.

	//! Populate the by_realm parameter as follows:
	//!
	//! Scheme   Type     by_realm
	//! ------   ----     --------
	//! basic    proxy    realm/[realm]
	//! basic    www      realm/[authority]/[realm]
	//! *        proxy    realm/[realm]
	//! *        www      realm/[authority]/[realm]
	//!
	//! Populate the by_uri parameter as follows:
	//!
	//! Scheme   Type     by_uri
	//! ------   ----     --------
	//! basic    proxy    uri
	//! basic    www      uri/[authority]
	//! *        proxy    uri/[authority]/[path]
	//! *        www      uri/[authority]/[path]
	//!
	//! Sets 'space' to proxy_authorization, www_authorization, or nullptr
	//! if resp is not an authentication challenge.
	//!
	//! \internal

	void get_default_protection_space(//! Request URI
					  const uriimpl &uri,
					  //! Authorization challenge
					  const responseimpl &resp,
					  //! Authentication scheme
					  auth scheme,
					  //! Realm
					  const std::string &realm,
					  //! Governing protection space
					  const protection_space_t *&space,
					  //! Realm-based hierarchy in space
					  std::list<std::string> &by_realm,

					  //! URI-based hierarchy in space
					  std::list<std::string> &by_uri)
		const;

	//! Process a failed authorization

	//! fail_authorization() calls this with an empty realm, to purge the
	//! any default basic authorization from the cache, then forwards its
	//! arguments again, in case a non-default cached authorization is no
	//! longer valid.
	//!
	//! \internal

	bool do_fail_authorization(//! Original request URI
				   const uriimpl &uri,

				   //! Received response
				   const responseimpl &resp,

				   //! The authorizations that were in the request
				   const clientauth &authorizations,

				   //! Challenge scheme
				   auth scheme,

				   //! Challenge realm
				   const std::string &realm,

				   //! Challenge parameters
				   const responseimpl::scheme_parameters_t
				   &params);


	//! Search for a failed authorization in a protection space

	//! \internal
	//!
	bool fail_authorization(const uriimpl &uri,
				const responseimpl &resp,
				std::map<std::string, clientauthimpl>
				&authorization_map,
				auth scheme,
				const std::string &realm,
				const responseimpl::scheme_parameters_t
				&params);
private:

	//! Common code for searching suitable proxy and www authorizations

	//! Finishes the job started by search_authorization().
	//! Appends uri's path to path, then searches a protection space for
	//! the entire path.
	void search_authorization_path(//! Original request
				       const requestimpl &req,

				       //! Root node of the protection space.
				       const protection_space_t::base::readlock
				       &lock,

				       //! Authorizations get added to this map
				       std::map<std::string, clientauthimpl>
				       &auth_map,

				       //! The initial prefix, uri & authority
				       std::list<std::string> &path)
		LIBCXX_HIDDEN;
};

#if 0
{
	{
#endif
	}
}
#endif
