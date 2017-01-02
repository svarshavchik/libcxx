/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "http_auth_internal.H"

#include "x/strtok.H"
#include "x/vector.H"
#include "x/tokens.H"
#include "x/uuid.H"
#include "x/tostring.H"
#include "x/http/serverauth.H"

#include <sstream>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::http::clientauthimplObj::digest);

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

void clientauthimplObj::digest
::parse_qop_params(const responseimpl::scheme_parameters_t &params,
		   qop_param_t &qop)
{
	auto qop_param=params.find("qop");

	if (qop_param == params.end())
		return;

	std::list<std::string> words;

	strtok_str(qop_param->second, ",", words);

	qop.clear();
	qop.insert(words.begin(), words.end());
}


clientauthimplObj::digest::digest_params_t::digest_params_t()
{
	parse(responseimpl::scheme_parameters_t());
}

void clientauthimplObj::digest::digest_params_t
::parse(const responseimpl::scheme_parameters_t &params_given)
{
	opaque_received=false;
	qop_auth=false;
	qop_auth_int=false;
	nonce_count=0;

	nonce.clear();

	{
		auto nonce_param=params_given.find("nonce");

		if (nonce_param != params_given.end())
			nonce=nonce_param->second;
	}

	{
		auto opaque_param=params_given.find("opaque");

		if (opaque_param != params_given.end())
		{
			opaque=opaque_param->second;
			opaque_received=true;
		}
	}

	{
		qop_param_t qop;

		parse_qop_params(params_given, qop);

		if (qop.find("auth") != qop.end())
			qop_auth=true;
		if (qop.find("auth-int") != qop.end())
			qop_auth_int=true;
	}
}

//! Digest object constructs this req in new_request(), and returns it.

class LIBCXX_HIDDEN clientauthimplObj::digest::req : public clientauthimplObj {

 public:

	//! Original digest object.

	ref<digest> digestref;

	//! The server nonce that we used
	std::string nonce;

	//! Whether qop auth flag was received
	bool qop_auth;

	//! Whether qop auth-int flag was received
	bool qop_auth_int;

	//! The nc that we sent
	std::string nc;

	//! The cnonce that we sent
	std::string cnonce;

	//! Constructor
	req(const ref<digest> &digestrefArg);

	//! Destructor
	~req();

	//! Add my header to a request
	void add_header(requestimpl &req, const char *header) override;

	//! Process the Authentication-Info header in the response.

	void authentication_info(const requestimpl &req,
				 const responseimpl::scheme_parameters_t &info)
		override;

	//! Invoke digest->get_protection_space()

	void get_protection_space(const responseimpl &resp,
				  const clientauthcacheObj &cache,
				  const protection_space_t *&space,
				  std::list< std::list<std::string> > &hier)
		override;

	//! This shouldn't be invoked, the original digest object shold get this

	clientauthimplptr new_request(const requestimpl &req) override;

	//! Process a stale request.

	clientauthimplptr stale(const uriimpl &request_uri,
				const responseimpl::scheme_parameters_t &req)
		override;

	//! Compute the digest when qop=auth is set.

	std::string compute_qop_digest(//! The unhashed A2 value
				       const std::string &a2);

	//! Compute the digest when qop=auth is not set.

	std::string compute_nonqop_digest(//! The unhashed A2 value
					  const std::string &a2);
};

clientauthimplObj::digest
::digest(const std::string &realmArg,
	 const std::set<uriimpl> &domainsArg,
	 const responseimpl::scheme_parameters_t &scheme_params,
	 gcry_md_algos algorithmArg,
	 const std::string &useridArg,
	 const std::string &passwordArg)
	: clientauthimplObj(auth::digest, realmArg), algorithm(algorithmArg),
	  userid(useridArg),
	  a1(serverauth::base::compute_a1(algorithmArg, useridArg, passwordArg,
					  realmArg)),
	  domains(domainsArg)
{
	params_t::lock lock(params);

	lock->parse(scheme_params);
}

clientauthimplObj::digest
::digest(const std::string &realmArg,
	 const std::set<uriimpl> &domainsArg,
	 const responseimpl::scheme_parameters_t &scheme_params,
	 const std::string &useridArg,
	 gcry_md_algos algorithmArg,
	 const std::string &a1Arg)
	: clientauthimplObj(auth::digest, realmArg), algorithm(algorithmArg),
	  userid(useridArg),
	  a1(a1Arg),
	  domains(domainsArg)
{
	params_t::lock lock(params);

	lock->parse(scheme_params);
}


clientauthimplObj::digest::~digest()
{
}

clientauthimplptr clientauthimplObj::digest
::new_request(const requestimpl &request)
{
	return ref<req>::create(ref<digest>(this));
}

void clientauthimplObj::digest::add_header(requestimpl &req,
					   const char *header)
{
	// Shouldn't end up here, this should end up getting called
	// on the req object's instance.
}

void clientauthimplObj::digest
::authentication_info(const requestimpl &req,
		      const responseimpl::scheme_parameters_t &info)
{
	// Shouldn't end up here, this should end up getting called
	// on the req object's instance.
}

clientauthimplptr
clientauthimplObj::digest::stale(const uriimpl &request_uri,
				 const responseimpl::scheme_parameters_t &req)
{
	// Shouldn't end up here, this should end up getting called
	// on the req object's instance.

	return clientauthimplptr();
}

void clientauthimplObj::digest
	::get_protection_space(const responseimpl &resp,
			       const clientauthcacheObj &cache,
			       const protection_space_t *&space,
			       std::list< std::list<std::string> > &hier_list)
{
	// Use get_default_protection_space() to obtain the protection space
	// for each URI covered by this digest authenticator, combine the list
	// (there may be dupes if there are multiple URIs in the same authority,
	// which results in the same by_realm URI.

	std::set<std::list<std::string> > combined_hier_list;

	for (const uriimpl &uri:domains)
	{
		std::list<std::string> by_realm, by_uri;

		cache.get_default_protection_space(uri, resp, auth::digest,
						   realm, space,
						   by_realm, by_uri);
		combined_hier_list.insert(by_realm);
		combined_hier_list.insert(by_uri);
	}

	hier_list.insert(hier_list.end(), combined_hier_list.begin(),
			 combined_hier_list.end());
}

clientauthimplObj::digest::req::req(const ref<digest> &digestrefArg)
	: clientauthimplObj(auth::digest, digestrefArg->realm),
	  digestref(digestrefArg)
{
}

clientauthimplObj::digest::req::~req()
{
}

void clientauthimplObj::digest::req
::get_protection_space(const responseimpl &resp,
		       const clientauthcacheObj &cache,
		       const protection_space_t *&space,
		       std::list< std::list<std::string> > &hier)
{
	return digestref->get_protection_space(resp, cache, space, hier);
}

clientauthimplptr clientauthimplObj::digest::req
::new_request(const requestimpl &req)
{
	// Shouldn't end up here, this should end up getting called
	// on the original digest object's instance.
	return clientauthimplptr();
}

clientauthimplptr clientauthimplObj::digest::req
::stale(const uriimpl &request_uri,
	const responseimpl::scheme_parameters_t &req)
{
	auto stale=req.find("stale");

	if (stale != req.end() &&
	    chrcasecmp::str_equal_to()(stale->second, "true"))
	{
		LOG_DEBUG("Stale nonce notification received, retrying");

		params_t::lock lock(digestref->params);

		lock->parse(req);

		return clientauthimplptr(this);
	}

	return clientauthimplptr();
}

void clientauthimplObj::digest::req::add_header(requestimpl &req,
						const char *header)
{
	std::map<std::string, std::pair<bool, std::string> > response;

	response.insert(std::make_pair("username",
				       std::make_pair(true, digestref->userid)));
	response.insert(std::make_pair("realm",
				       std::make_pair(true, digestref->realm)));

	std::string digest_uri=req.getURI().getPath();

	if (digest_uri.empty())
		digest_uri="/";

	response.insert(std::make_pair("uri",
				       std::make_pair(true,
						      digest_uri)));
	response.insert(std::make_pair("algorithm",
				       std::make_pair(false,
						      gcrypt::md::base
						      ::name(digestref->algorithm))));

	{
		params_t::lock lock(digestref->params);

		qop_auth=lock->qop_auth;
		qop_auth_int=lock->qop_auth_int;

		if (lock->qop_auth)
		{
			++lock->nonce_count;

			std::ostringstream cnonce_str;

			cnonce_str << tostring(uuid());
			cnonce=cnonce_str.str();
		}

		response.insert(std::make_pair("nonce",
					       std::make_pair(true,
							      lock->nonce)));

		if (lock->opaque_received)
			response.insert(std::make_pair("opaque",
						       std::make_pair(true,
								      lock->
								      opaque)));
	// auth, auth-int
	// nc

		auto a2=std::string(requestimpl::methodstr(req.getMethod()))
			+ ":" + digest_uri;

		nonce=lock->nonce;

		if (lock->qop_auth)
		{
			response.insert(std::make_pair("qop",
						       std::make_pair(false,
								      "auth"))
					);

			nc=nonce_count_to_hex(lock->nonce_count);

			response.insert(std::make_pair("nc",
						       std::make_pair(false,
								      nc)));

			response.insert(std::make_pair("cnonce",
						       std::make_pair(true,
								      cnonce)));
		}

		std::string request_digest=lock->qop_auth ?
			compute_qop_digest(a2) : compute_nonqop_digest(a2);

		response.insert(std::make_pair
				("response",
				 std::make_pair(true, request_digest)));
	}

	std::ostringstream o;

	o << auth_tostring(auth::digest);

	const char *sep=" ";

	for (const auto &param:response)
	{
		o << sep << param.first << '=';

		if (param.second.first)
			headersbase::emit_quoted_string
				(std::ostreambuf_iterator<char>(o),
				 param.second.second.begin(),
				 param.second.second.end());
		else
			tokenizer<is_http_token>
				::emit_token_or_quoted_word
				(std::ostreambuf_iterator<char>(o),
				 param.second.second.begin(),
				 param.second.second.end());
		sep=", ";
	}

	LOG_TRACE(header << ": " << o.str());
	req.append(header, o.str());
}

std::string clientauthimplObj::digest::req
::compute_qop_digest(const std::string &a2)
{
	return compute_rfc2617_digest_auth(digestref->algorithm,
					   digestref->a1,
					   nonce,
					   nc,
					   cnonce,
					   "auth",
					   a2);
}

std::string clientauthimplObj::digest::req
::compute_nonqop_digest(const std::string &a2)
{
	return compute_rfc2069_digest(digestref->algorithm,
				      digestref->a1,
				      nonce,
				      a2);
}

void clientauthimplObj::digest::req
::authentication_info(const requestimpl &req,
		      const responseimpl::scheme_parameters_t &info)
{
	qop_param_t qop;

	parse_qop_params(info, qop);

	if (qop_auth)
	{
		auto rsp_auth=info.find("rspauth");

		if (rsp_auth == info.end())
			return;

		std::string digest_uri=req.getURI().getPath();

		if (digest_uri.empty())
			digest_uri="/";

		std::string a2=":" + digest_uri;

		auto iter=info.find("nc");

		if (iter == info.end() || iter->second != nc)
		{
			LOG_WARNING("nc in Authentication-Info: mismatch");
			return;
		}

		iter=info.find("cnonce");

		if (iter == info.end() || iter->second != cnonce)
		{
			LOG_WARNING("cnonce in Authentication-Info: mismatch");
			return;
		}

		std::string computed_rspauth=compute_qop_digest(a2);

		if (computed_rspauth != rsp_auth->second)
		{
			LOG_WARNING("rspauth in Authentication-Info: mismatch");
			return;
		}
	}

	auto nextnonce=info.find("nextnonce");

	if (nextnonce != info.end())
	{
		params_t::lock lock(digestref->params);

		lock->nonce=nextnonce->second;
		lock->nonce_count=0;

		LOG_DEBUG("nonce reset from Authentication-Info");
	}
}

// ----------------------------------------------------------------------------

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::http::digest, digestLog);

static std::string compute_digest(gcry_md_algos algorithm,
				  const std::string &h,
				  const std::string &a2)
{
	LOG_FUNC_SCOPE(digestLog);

	LOG_TRACE("Computing " << gcrypt::md::base::name(algorithm)
		  << ", digest for \"" << h << "\" with A2=\""
		  << a2 << "\"" << std::endl);

	std::string v = h + gcrypt::md::base::hexdigest(a2.begin(),
							a2.end(),
							algorithm);

	return gcrypt::md::base::hexdigest(v.begin(),
					   v.end(),
					   algorithm);
}

std::string compute_rfc2617_digest_auth(gcry_md_algos algorithm,
					const std::string &a1,
					const std::string &nonce,
					const std::string &nc,
					const std::string &cnonce,
					const std::string &qop,
					const std::string &a2)
{
	return compute_digest(algorithm,
			      a1 + ":" + nonce + ":" + nc + ":"
			      + cnonce + ":" + qop + ":", a2);
}

std::string compute_rfc2069_digest(gcry_md_algos algorithm,
				   const std::string &a1,
				   const std::string &nonce,
				   const std::string &a2)
{
	return compute_digest(algorithm, a1 + ":" + nonce + ":", a2);
}

std::string nonce_count_to_hex(uint32_t nonce_count)
{
	std::ostringstream nonce_count_str;

	nonce_count_str << std::hex
			<< std::setw(8)
			<< std::setfill('0')
			<< nonce_count;

	return nonce_count_str.str();
}

#if 0
{
	{
#endif
	};
};

