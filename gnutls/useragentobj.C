/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/useragent.H"
#include "x/http/fdtlsclientobj.H"
#include "x/http/clientauthimpl.H"
#include "x/gnutls/credentials.H"
#include "x/logger.H"
#include "x/messages.H"
#include "x/gcrypt/md.H"
#include "x/strtok.H"
#include "x/join.H"
#include "gettext_in.h"

#include "http_auth_internal.H"

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

http::useragentObj::idleconn
http::useragentObj::init_https_socket(clientopts_t opts,
				      const std::string &host,
				      const fd &socket,
				      const fdptr &terminator)
{
	ref<connObj<gnutls::http::fdtlsclientObj> >
		newclient(ref<connObj<gnutls::http::fdtlsclientObj> >
			  ::create());

	gnutls::credentials::certificate
		cert(gnutls::credentials::certificate::create());

	cert->set_x509_trust_default();

	gnutls::session sess(gnutls::session::create(GNUTLS_CLIENT,
						     socket));
	sess->credentials_set(cert);
	sess->set_default_priority();
	sess->set_private_extensions();

	newclient->install(sess, socket, terminator, opts);
	return newclient;
}

void http::useragentBase::https_enable() noexcept
{
	(void)&http::useragentObj::init_https_socket;
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::http::useragentObj::challengeObj::create_digest,
		    createDigestLog);

class LIBCXX_HIDDEN http::useragentObj::challengeObj::digestObj
	: public challengeObj {

 public:
	std::string realm;
	std::set<uriimpl> protection_space;
	gcry_md_algos algorithm_number;
	responseimpl::scheme_parameters_t params;

	digestObj(const std::string &realmArg,
		  const std::set<uriimpl> &protection_spaceArg,
		  gcry_md_algos algorithmArg,
		  const responseimpl::scheme_parameters_t &paramsArg)
		: challengeObj(auth::digest,
			       gcry_md_get_algo_dlen(algorithmArg)),
		realm(realmArg),
		protection_space(protection_spaceArg),
		algorithm_number(algorithmArg),
		params(paramsArg)
		{
		}
	~digestObj()
	{
	}

	clientauthimpl create(const std::string &username,
			      const std::string &password) override
	{
		return ref<clientauthimplObj::digest>
			::create(realm, protection_space, params,
				 algorithm_number, username, password);
	}

	clientauthimpl create_hash(const std::string &username,
				   const std::string &a1_hash) override
	{
		return ref<clientauthimplObj::digest>
			::create(realm, protection_space, params,
				 username, algorithm_number, a1_hash);
	}

	gcry_md_algos algorithm()
	{
		return algorithm_number;
	}
};

http::useragent::base::challengeptr http::useragentObj::challengeObj
::create_digest(const uriimpl &request_uri,
		const std::string &realm,
		const responseimpl::scheme_parameters_t &params)
{
	LOG_FUNC_SCOPE(createDigestLog);

	std::set<uriimpl> protection_space;
	gcry_md_algos algorithm;

	for (auto domains=params.equal_range("domain");
	     domains.first != domains.second; ++domains.first)
	{
		std::list<std::string> uris;

		strtok_str(domains.first->second, " ", uris);

		for (const std::string &domain:uris)
		{
			// This resolves any relative URIs in the domain
			// parameter. An absolute domain overrides request_uri
			// completely.
			protection_space.insert(request_uri + domain);
		}
	}

	if (protection_space.empty())
	{
		LOG_WARNING(libmsg()->get(_txt("Domain parameter missing from digest authentication challenge")));

		return challengeptr();
	}

	std::string algorithm_name="md5";

	auto algorithm_iter=params.find("algorithm");

	if (algorithm_iter != params.end())
		algorithm_name=algorithm_iter->second;

	try {
		algorithm=gcrypt::md::base::number(algorithm_name);
	} catch (const x::exception &e)
	{
		std::ostringstream o;

		o << e;

		LOG_WARNING(o.str());
		return challengeptr();
	}

	LOG_DEBUG("Installing " + gcrypt::md::base::name(algorithm)
		  + " digest authentication for: "
		  + ({
				  std::set<std::string> uris;

				  for (const auto &uri:protection_space)
				  {
					  uris.insert(tostring(uri));
				  }

				  join(uris, ", ");
			  }));

	return ref<http::useragentObj::challengeObj::digestObj>
		::create(realm, protection_space, algorithm, params);
}

#if 0
{
#endif
};
