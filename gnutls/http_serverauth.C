/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/serverauth.H"
#include "x/property_value.H"
#include "x/hms.H"
#include "x/strtok.H"
#include "x/join.H"
#include "x/logger.H"
#include <list>
#include <sstream>
#include <algorithm>

#include "http_auth_internal.H"

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

std::string serverauthBase::compute_a1(gcry_md_algos algorithm,
				       const std::string &userid,
				       const std::string &password,
				       const std::string &realm)
{
	std::string a1_str = userid + ":" + realm + ":" + password;

	return gcrypt::md::base::hexdigest(a1_str.begin(),
					   a1_str.end(),
					   algorithm);
}

static property::value<hms>
default_nonce_expiration(LIBCXX_NAMESPACE_STR
			 "::http::serverauth::nonce_expiration",
			 hms(0, 1, 0));

static property::value<std::string>
default_algorithms(LIBCXX_NAMESPACE_STR
		   "::http::serverauth::algorithms", "MD5");

serverauthObj::serverauthObj(const std::string &realmArg,
			     const std::set<uriimpl> &domainArg)
	: realm(realmArg), domain(domainArg),
	  nonce_expiration(default_nonce_expiration.get().seconds())
{
	std::list<std::string> algorithm_strs;

	strtok_str(default_algorithms.get(), " \t\r\n,", algorithm_strs);

	for (const auto &name:algorithm_strs)
	{
		try {
			algorithms.push_back(gcrypt::md::base::number(name));
		} catch (...)
		{
		}
	}

	if (algorithms.empty())
		algorithms.push_back(GCRY_MD_MD5);
}

serverauthObj::~serverauthObj()
{
}

void serverauthObj::add_uri(std::set<uriimpl> &local_uris,
			    std::set<uriimpl> &abs_uris,
			    const uriimpl &nextUri)
{
	(nextUri.getAuthority() ? abs_uris:local_uris).insert(nextUri);
}

void serverauthObj::compute_uris(std::set<uriimpl> &local_uris,
				 std::set<uriimpl> &abs_uris)
{
	domain=local_uris;

	for (const auto &abs:abs_uris)
		for (const auto &local:local_uris)
			domain.insert(abs+local);
}

void serverauthObj::build_challenges(std::list<responseimpl::challenge_info> &challenges)
{
	build_challenges(challenges, false, clock_t::now());
}

void serverauthObj::build_challenges(std::list<responseimpl::challenge_info> &challenges,
				     bool stale,
				     const clock_t::time_point &timestamp)
{
	std::string domains=({
			std::vector<std::string> strbuf;

			strbuf.resize(domain.size());

			std::transform(domain.begin(), domain.end(),
				       strbuf.begin(),
				       []
				       (const uriimpl &u)
				       {
					       return to_string(u);
				       });

			headersbase::quoted_string(join(strbuf, " "));
		});

	std::string nonce=({
			std::ostringstream o;

			o << clock_t::now().time_since_epoch().count();

			headersbase::quoted_string(o.str());
		});

	challenges.clear();

	for (auto algorithm:algorithms)
	{
		responseimpl::scheme_parameters_t
			resp({
					{"domain", domains},
					{"nonce", nonce},
					{"algorithm", gcrypt::md::base
							::name(algorithm)},
					{"qop", "\"auth\""},
				});

		if (stale)
			resp.insert(std::make_pair("stale", "yes"));

		challenges.emplace_back(auth::digest, realm, resp);
	}
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::http::serverauth::authorization,
		    authorizationLog);

auth serverauthObj::authorization(const char *header,
				  const requestimpl &req,
				  authorization_request &info)
{
	LOG_FUNC_SCOPE(authorizationLog);

	for (auto authorization_header=req.equal_range(header);
	     authorization_header.first != authorization_header.second;
	     ++authorization_header.first)
	{
		auto b=authorization_header.first->second.begin();
		auto e=authorization_header.first->second.end();

		LOG_DEBUG("Parsing header: " << std::string(b, e));
		if (req.is_basic_auth(b, e, info.username))
		{
			LOG_DEBUG("Returned basic authorization");
			return auth::basic;
		}

		auth scheme=auth::unknown;
		bool first=true;

		responseimpl::scheme_parameters_t params;

		req.parse_structured_header
			([&]
			 (char c)
			 {
				 // Expect "digest", followed by comma-
				 // separator parameters. Use whitespace
				 // as the delimiter for the first word,
				 // then commas for the rest.

				 return c == ',' ||
					 (first && (c == ' ' ||
						    c == '\r' ||
						    c == '\n' ||
						    c == '\t'));
			 },
			 [&]
			 (bool ignore,
			  const std::string &word)
			 {
				 if (first)
				 {
					 LOG_TRACE("Scheme: " << word);
					 first=false;
					 scheme=auth_fromstring(word);
					 return;
				 }

				 if (scheme == auth::digest)
				 {
					 LOG_TRACE("Parameter: " << word);

					 size_t p=word.find('=');

					 if (p != word.npos)
						 params.insert
							 (std::make_pair
							  (word.substr(0, p),
							   word.substr(p+1)));
				 }
			 }, b, e);

		if (scheme != auth::digest)
			continue;

		responseimpl::scheme_parameters_t::iterator iter;

		if ((iter=params.find("realm")) == params.end())
		{
			LOG_DEBUG("Realm not found");
			continue;
		}

		if (iter->second != realm)
		{
			LOG_DEBUG("Realm " << iter->second << " is not me");
			continue;
		}

		info.nonce_count=0;
		info.nonce=clock_t::time_point();
		info.qop_auth.clear();

		if ((iter=params.find("username")) == params.end())
		{
			LOG_DEBUG("Username not found");
			return auth::unknown;
		}

		info.username=iter->second;

		if ((iter=params.find("nonce")) == params.end())
		{
			LOG_DEBUG("Nonce not found");
			return auth::unknown;
		}

		{
			std::istringstream i(iter->second);

			clock_t::duration::rep val;

			i >> val;

			if (i.fail() || !i.eof())
			{
				LOG_DEBUG("Nonce value cannot be parsed");
				return auth::unknown;
			}
			info.nonce=clock_t::time_point(clock_t::duration(val));
		}

		if ((iter=params.find("uri")) == params.end())
		{
			LOG_DEBUG("Uri not found");
			return auth::unknown;
		}

		info.uri=iter->second;

		info.algorithm=GCRY_MD_MD5;

		if ((iter=params.find("algorithm")) != params.end())
		{
			try {
				info.algorithm=gcrypt::md::base
					::number(iter->second);
			} catch (const exception &e)
			{
				LOG_DEBUG(e);
				return auth::unknown;
			}

			if (std::find(algorithms.begin(), algorithms.end(),
				      info.algorithm) == algorithms.end())
			{
				LOG_DEBUG("Unknown algorithm specified");
				return auth::unknown;
			}
		}

		if ((iter=params.find("response")) == params.end())
		{
			LOG_DEBUG("Nonce not found");
			return auth::unknown;
		}
		info.response=iter->second;

		iter=params.find("qop");

		if (iter != params.end())
		{
			if (! chrcasecmp::str_equal_to()(iter->second, "auth"))
			{
				LOG_DEBUG("qop value not known");
				return auth::unknown;
			}

			info.qop_auth=iter->second;

			if ((iter=params.find("cnonce")) == params.end())
			{
				LOG_DEBUG("cnonce not found");
				return auth::unknown;
			}
			info.cnonce=iter->second;

			if ((iter=params.find("nc")) == params.end())
			{
				LOG_DEBUG("nc not found");
				return auth::unknown;
			}

			{
				std::istringstream i(iter->second);

				i >> std::hex >> info.nonce_count;

				if (i.fail() || !i.eof())
				{
					LOG_DEBUG("nc value cannot be parsed");
					return auth::unknown;
				}
			}
		}
		return auth::digest;
	}

	LOG_DEBUG("Header " << header << " not found");

	return auth::unknown;
}

serverauthObj::authorization_request::authorization_request()
{
}

auth serverauthObj::doDigestScheme(const requestimpl &req,
				   std::list<responseimpl::
				   challenge_info> &challenges,
				   const char *authentication_header,
				   responseimpl &resp,
				   const std::string &a1,
				   const authorization_request &info)
{
	auto a2=std::string(requestimpl::methodstr(req.getMethod()))
		+ ":" + info.uri;

	std::string nonce_str=to_string(info.nonce.time_since_epoch().count());
	std::string nonce_count_str=nonce_count_to_hex(info.nonce_count);

	clock_t::time_point now=clock_t::now();
	bool stale=false;

	if (!a1.empty() &&
	    (info.qop_auth.empty() ?
	     compute_rfc2069_digest(info.algorithm, a1, nonce_str, a2) :
	     compute_rfc2617_digest_auth(info.algorithm, a1, nonce_str,
					 nonce_count_str,
					 info.cnonce,
					 info.qop_auth, a2)) == info.response)
	{
		if (info.nonce <= now &&
		    info.nonce >= now -
		    std::chrono::duration_cast<clock_t::duration>
		    (std::chrono::seconds(nonce_expiration)))
		{
			// If the client's nonce is less than one second old,
			// the client does not need another authentication-info
			// header

			if (info.nonce >= now-std::chrono::duration_cast
			    <clock_t::duration>(std::chrono::seconds(1)))
				return auth::digest;

			std::ostringstream o;

			o << "algorithm="
			  << gcrypt::md::base::name(info.algorithm)
			  << ", nextnonce=\"" << to_string(now.time_since_epoch()
							  .count()) << "\"";

			if (!info.qop_auth.empty())
			{
				o << ", qop=" << info.qop_auth
				  << ", cnonce=\"" << to_string(info.cnonce)
				  << "\", nc=\"" << nonce_count_str
				  << "\", rspauth=\""
				  << compute_rfc2617_digest_auth
					(info.algorithm, a1, nonce_str,
					 nonce_count_str,
					 info.cnonce,
					 info.qop_auth, ":" + info.uri)
				  << "\"";
			}

			resp.append(authentication_header, o.str());
			return auth::digest;
		}
		stale=true;
	}

	build_challenges(challenges, stale, now);

	return auth::unknown;
}

#if 0
{
	{
#endif
	};
};
