/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_serverauthobj_H
#define x_http_serverauthobj_H

#include <x/obj.H>
#include <x/uriimpl.H>
#include <x/http/requestimpl.H>
#include <x/http/responseimpl.H>
#include <x/http/serverauthfwd.H>
#include <x/gcrypt/md.H>
#include <x/logger.H>
#include <x/namespace.h>

#include <list>
#include <chrono>
namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

//! Implement HTTP authentication-related functionality on the server

class serverauthObj : virtual public obj {

public:
	//! Server realm
	std::string realm;

	//! Protection space
	std::set<uriimpl> domain;

	//! Supported algorithms.

	//! Defaults to just MD5, that's the only one defined by RFC 2617
	//! (we don't implement MD5-Sess, yet). This defaults to a property
	//! (see the source), and you can add additional algorithms immediately
	//! after constructing, for specialized applications where clients can
	//! be vetted for proper support (buggy clients are still too
	//! prevalent, at press time).
	//!
	//! In theory, this should be least-to-most preferred, but, if
	//! multiple algorithms get employed, at this time it should be the
	//! other way around, sadly.

	std::list<gcry_md_algos> algorithms;

	//! The clock used to generate nonces

	typedef std::chrono::steady_clock clock_t;

	//! nonce expiration

	//! Nonces expire after this amount of time elapses. This defaults to
	//! 60 seconds, which is ok. As long as there's at least one request
	//! a minute (which should be sufficient for most password prompts,
	//! and page loading) we'll send a new nonce to the client,
	//! automatically.

	unsigned nonce_expiration;

	//! Constructor

	serverauthObj(const std::string &realmArg,
		      const std::set<uriimpl> &domainArg);

	//! Construct domains from a variadic list

	//! Instead of building the domain space yourself, pass a variadic
	//! list of non-authority and authority URIs, like "/private", "/cgi",
	//! "http://example.com", "https://example.com".
	//!
	//! This computes a domain space consisting of all the non-authority
	//! URIs, and a cross-product of each non-authority URI with an
	//! authority URI.

	template<typename ...Args>
	serverauthObj(const std::string &realmArg,
		      const uriimpl &firstUri,
		      Args && ...args) : serverauthObj(realmArg,
						       std::set<uriimpl>())
	{
		std::set<uriimpl> local_uris;
		std::set<uriimpl> abs_uris;

		build_uris(local_uris, abs_uris, firstUri,
			   std::forward<Args>(args)...);

		compute_uris(local_uris, abs_uris);
	}

private:

	//! End of variadic URI list
	void build_uris(std::set<uriimpl> &local_uris,
			std::set<uriimpl> &abs_uris)
	{
	}

	//! Process the next element in the URI list.

	template<typename ...Args>
	void build_uris(std::set<uriimpl> &local_uris,
			std::set<uriimpl> &abs_uris,
			const uriimpl &nextUri,
			Args && ...args)
	{
		add_uri(local_uris, abs_uris, nextUri);
		build_uris(local_uris, abs_uris,
			   std::forward<Args>(args)...);
	}

	//! Add next URI to either the local or the absolute URI set.
	void add_uri(std::set<uriimpl> &local_uris,
		     std::set<uriimpl> &abs_uris,
		     const uriimpl &nextUri);

	//! Calculate and set the domain space.

	void compute_uris(std::set<uriimpl> &local_uris,
			  std::set<uriimpl> &abs_uris);

public:

	//! Destructor
	~serverauthObj() noexcept;

	//! Build an authentication challenge

	void build_challenges(//! Initializes with a list of challenges.
			      std::list<responseimpl::challenge_info> &challenges);

private:
	//! Build an authentication challenge

	//! \internal
	void build_challenges(std::list<responseimpl::challenge_info> &challenges,
			      bool stale,
			      const clock_t::time_point &timestamp);

public:
	//! A parsed authorization request

	struct authorization_request {

		//! Constructor
		authorization_request();

		//! username parameter
		std::string username;

		//! Nonce
		clock_t::time_point nonce;

		//! uri parameter.
		std::string uri;

		//! algorithm parameter
		gcry_md_algos algorithm;

		//! response parameter
		std::string response;

		//! The qop parameter, either "auth", or an empty strnig.
		std::string qop_auth;

		//! nonce_count parameter, if qop_auth is set
		uint32_t nonce_count;

		//! cnonce parameter, if qop_auth is set
		std::string cnonce;
	};

	//! Parse an Authorization: header

	//! Returns auth::basic if the request is a basic authorization scheme,
	//! returning the base64-decoded "username:password" string in the
	//! username value, and all other values not initialized. Returns
	//! auth::digest if the request is a digest authorization scheme, with
	//! the rest of the values initialized as documented.
	//!
	//! Returns auth::unknown if no authorization header was specified, or
	//! if any required parameters were missing. Returns auth::unknown
	//! if certain parameters were detected as being misformatted:
	//!
	//! - required parameters were not specified.
	//!
	//! - realm parameter is not the same as the realm passed to the
	//! constructor.
	//!
	//! - algorithm is not found in algorithms.
	//!
	//! - nonce is not numeric (our nonces are numeric)
	//!
	//! - nonce_count is not hexadecimal (if applicable)
	//!
	//! \internal

	auth authorization(//! requestimpl::www_authorization or requestimpl::proxy_authorization
			   const char *header,

			   //! The request
			   const requestimpl &req,

			   //! Parsed authorization request
			   authorization_request &info);

	//! Validate digest authorization header.

	//! If the authorization is valid, sets authentication_info,
	//! and returns auth::digest.
	//!
	//! Returns auth::unknown if the authorization failed, and adds
	//! a challenge to challenges.
	//!
	//! \internal

	auth doDigestScheme(//! Original request
			    const requestimpl &req,

			    //! Populated for auth::unknown result

			    std::list<responseimpl::challenge_info> &challenges,

			    //! responseimpl::authentication_info or responseimpl::proxy_authentication_info,
			    const char *authentication_header,

			    //! The tentative response
			    responseimpl &resp,

			    //! A1 value for info.username
			    const std::string &a1,

			    //! Parsed Authorization: header.
			    const authorization_request &info);

private:

	//! Process Authorization: or Proxy-Authorization: header.

	template<typename getA1Functor, typename doBasicSchemeFunctor>
	auth do_check_authentication(//! requestimpl::www_authorization or requestimpl::proxy_authorization
				    const char *header,

				    //! responseimpl::authentication_info or responseimpl::proxy_authentication_info,
				    const char *authentication_header,

				    //! The request
				    const requestimpl &req,

				    //! The tentative response
				    responseimpl &resp,

				    //! Authenticated ID
				    std::string &username,

				    //! Returned authentication response.
				    std::list<responseimpl::challenge_info> &challenges,

				    //! Functor to return the A1 string for digest auth
				    getA1Functor && getA1,

				    //! Process basic authentication functor
				    doBasicSchemeFunctor && doBasicScheme)
	{
		authorization_request info;

		switch (authorization(header, req, info)) {
		case auth::basic:
			username=info.username;
			if (doBasicScheme(username))
				return auth::basic;
			break;
		case auth::digest:
			username=info.username;

			return doDigestScheme(req, challenges,
					      authentication_header, resp,
					      getA1(info.algorithm,
						    realm, username,
						    info.uri),
					      info);
		default:
			break;
		}
		build_challenges(challenges);
		return auth::unknown;
	}

public:
	//! Process Authorization: or Proxy-Authorization: header.

	template<typename getA1Functor, typename doBasicSchemeFunctor>
	auth check_auth_header(//! requestimpl::www_authorization or requestimpl::proxy_authorization
			       const char *header,

			       //! responseimpl::authentication_info or responseimpl::proxy_authentication_info,
			       const char *authentication_header,

			       //! The request
			       const requestimpl &req,

			       //! The tentative response
			       responseimpl &resp,

			       //! Authenticated ID
			       std::string &username,

			       //! Returned authentication response.
			       std::list<responseimpl::challenge_info> &challenges,
			       //! Functor to return the A1 string for digest auth
			       getA1Functor && getA1,

			       //! Process basic authentication functor
			       doBasicSchemeFunctor && doBasicScheme)
	{
		auto rc=do_check_authentication(header,
						authentication_header,
						req, resp, username,
						challenges,
						std::forward<getA1Functor>
						(getA1),
						std::forward<doBasicSchemeFunctor
						>(doBasicScheme));

		if (rc == auth::unknown)
			challenges.emplace_back(auth::basic,
						realm,
						responseimpl
						::scheme_parameters_t());
		return rc;
	}

	//! Process Authorization: or Proxy-Authorization: header.

	//! Accept a digest scheme only.

	template<typename getA1Functor>
	auth check_auth_header(//! requestimpl::www_authorization or requestimpl::proxy_authorization
			       const char *header,

			       //! responseimpl::authentication_info or responseimpl::proxy_authentication_info,
			       const char *authentication_header,

			       //! The request
			       const requestimpl &req,

			       //! The tentative response
			       responseimpl &resp,

			       //! Authenticated ID
			       std::string &username,

			       //! Returned authentication response.
			       std::list<responseimpl::challenge_info> &challenges,
			       //! Functor to return the A1 string for digest auth
			       getA1Functor && getA1)
	{
		return do_check_authentication(header,
					       authentication_header,
					       req, resp, username,
					       challenges,
					       std::forward<getA1Functor>
					       (getA1),
					       []
					       (std::string &userpw)
					       {
						       return false;
					       });
	}

public:

	//! Verify www_authorization header.

	//! \see check_auth_header

	template<typename ...Args>
	auth check_authentication(Args && ...args)
	{
		return check_auth_header(requestimpl::www_authorization,
					 responseimpl::authentication_info,
					 std::forward<Args>(args)...);
	}

	//! Verify proxy_authorization header.

	//! \see check_auth_header

	template<typename ...Args>
	auth check_proxy_authentication(Args && ...args)
	{
		return check_auth_header(requestimpl::proxy_authorization,
					 responseimpl::proxy_authentication_info,
					 std::forward<Args>(args)...);
	}

};

#if 0
{
	{
#endif
	}
}

#endif
