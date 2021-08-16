/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef gnutls_http_auth_internal_H
#define gnutls_http_auth_internal_H

#include "x/http/clientauth.H"
#include "x/http/responseimpl.H"
#include "x/gcrypt/md.H"
#include "x/logger.H"
#include "x/mpobj.H"
#include "x/chrcasecmp.H"
#include "x/namespace.h"

#include <set>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

//! Compute rfc2617 digest when qop=auth is set.

std::string compute_rfc2617_digest_auth(//! Algorithm
					gcry_md_algos algorithm,

					//! a1 parameter
					const std::string &a1,

					//! nonce parameter
					const std::string &nonce,

					//! nc parameter
					const std::string &nc,

					//! cnonce parameter
					const std::string &cnonce,

					//! Exact caseing of qop
					const std::string &qop,

					//! Unhashed a2 parameter
					const std::string &a2) LIBCXX_HIDDEN;

//! Compute rfc2069 digest

std::string compute_rfc2069_digest(//! Algorithm
				   gcry_md_algos algorithm,

				   //! a1 parameter
				   const std::string &a1,

				   //! nonce parameter
				   const std::string &nonce,

				   //! a2 parameter
				   const std::string &a2) LIBCXX_HIDDEN;

//! Basic authentication scheme implementation.

class LIBCXX_HIDDEN clientauthimplObj::digest : public clientauthimplObj {

	//! The logging object.
	LOG_CLASS_SCOPE;

 public:

	//! Algorithm
	gcry_md_algos algorithm;

	//! Userid
	const std::string userid;

	//! The A1 string
	const std::string a1;

	//! This realm's protection space
	std::set<uriimpl> domains;

	//! Other digest parameters.

	class digest_params_t {
	public:
		//! Most recently-received nonce
		std::string nonce;

		//! Whether the opaque parameter was received
		bool opaque_received;

		//! The opaque parameter in the server's challenge
		std::string opaque;

		//! Whether qop auth flag was received
		bool qop_auth;

		//! Whether qop auth-int flag was received
		bool qop_auth_int;

		//! nonce counter, set to 0 by parse().
		uint32_t nonce_count;

		//! Constructor
		digest_params_t();

		//! Initialize based on scheme_parameters_t

		void parse(const responseimpl::scheme_parameters_t &);
	};

	//! The container for the most recently received parameters
	typedef mpobj<digest_params_t> params_t;

	//! Most recently received parameters, mutex-protected

	params_t params;

	//! The container used for parsing the qop parameter

	typedef std::set<std::string, chrcasecmp::str_less> qop_param_t;

	//! Parse the qop parameter

	static void parse_qop_params(const responseimpl::scheme_parameters_t &params,
				     qop_param_t &qop);


	//! Constructor
	digest(const std::string &realm,
	       const std::set<uriimpl> &domainsArg,
	       const responseimpl::scheme_parameters_t &params,
	       gcry_md_algos algorithmArg,
	       const std::string &useridArg,
	       const std::string &passwordArg);

	//! Constructor
	digest(const std::string &realm,
	       const std::set<uriimpl> &domainsArg,
	       const responseimpl::scheme_parameters_t &params,
	       const std::string &useridArg,
	       gcry_md_algos algorithmArg,
	       const std::string &a1Arg);
	~digest();

	//! Add my header to a request
	void add_header(requestimpl &req, const char *header) override;

	//! Process the Authentication-Info header in the response.

	void authentication_info(const requestimpl &req,
				 const responseimpl::scheme_parameters_t &info)
		override;

	//! Return my protection space.
	void get_protection_space(const responseimpl &resp,
				  const clientauthcacheObj &cache,
				  const protection_space_t *&space,
				  std::list< std::list<std::string> > &hier)
		override;


	//! Add this authenticator to a new request.

	clientauthimplptr new_request(const requestimpl &req) override;

	//! This shouldn't be invoked, new_request's object should get this.

	clientauthimplptr stale(const uriimpl &request_uri,
				const responseimpl::scheme_parameters_t &req)
		override;

	class req;
};

//! Convert a nonce count to 8LHEX

std::string nonce_count_to_hex(uint32_t nonce_count) LIBCXX_INTERNAL;

#if 0
{
#endif
}
#endif
