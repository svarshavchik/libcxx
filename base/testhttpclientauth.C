/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "http_clientauthobj.C"
#include "http_clientauthcacheobj.C"
#include "http_clientauthimplobj.C"
#include "messages.C"
#include "x/http/useragent.H"
#include "x/join.H"
#include "x/strtok.H"
#include "x/options.H"

#include <iostream>

class dummyauthObj : public LIBCXX_NAMESPACE::http::clientauthimplObj {

public:
	std::string id;

	dummyauthObj(const std::string &idArg,
		     LIBCXX_NAMESPACE::http::auth schemeArg,
		     const std::string &realmArg)
		: LIBCXX_NAMESPACE::http::clientauthimplObj(schemeArg, realmArg),
		  id(idArg)
	{
	}

	~dummyauthObj() {}

	void add_header(LIBCXX_NAMESPACE::http::requestimpl &req,
			const char *header) override
	{
		req.append(header, id);
	}
};

class LIBCXX_HIDDEN testclientauthcacheObj
	: public LIBCXX_NAMESPACE::http::clientauthcacheObj {

public:
	using LIBCXX_NAMESPACE::http::clientauthcacheObj::decompose_authority;
	using LIBCXX_NAMESPACE::http::clientauthcacheObj::decompose_path;

	testclientauthcacheObj() {}

	~testclientauthcacheObj() {}

	std::string to_string()
	{
		return dump(proxy_authorizations) + "|" +
			dump(www_authorizations);
	}

static std::string dump(const LIBCXX_NAMESPACE::http::clientauthcacheObj
			::protection_space_t &ps)
	{
		std::list<std::string> realms;

		for (const auto &hier: *ps)
		{
			realms.push_back(hier.empty() ? "":
					 ("/" + LIBCXX_NAMESPACE::join(hier,
								       "/")));
		}

		return LIBCXX_NAMESPACE::join(realms, ",");
	}
};

void testuridecode()
{
	static const char * const uris[]={
		"http://example.com",
		"HTTP://example.com/",
		"http://EXAMPLE.COM:80",
		"http://example.com/foo/bar",
		"http://example.com/foo/bar/",
	};

	for (const auto &uri:uris)
	{
		std::list<std::string> path;

		path.push_back(testclientauthcacheObj
			       ::decompose_authority(uri));
		testclientauthcacheObj::decompose_path(uri, path);

		std::cout << "path:";
		for (const auto &component:path)
		{
			std::cout << " [" << component << "]";
		}
		std::cout << std::endl;
	}
}

typedef LIBCXX_NAMESPACE::http::clientauthcache clientauthcache;
typedef LIBCXX_NAMESPACE::http::clientauthimpl clientauthimpl;

#define www_auth_code \
	LIBCXX_NAMESPACE::http::responseimpl::www_authenticate_code

#define proxy_auth_code \
	LIBCXX_NAMESPACE::http::responseimpl::proxy_authenticate_code


void authsave(const clientauthcache &cache,
	      int code,
	      const LIBCXX_NAMESPACE::uriimpl &uri,
	      const std::string &realm,
	      const std::string &name,
	      LIBCXX_NAMESPACE::http::auth a=
	      LIBCXX_NAMESPACE::http::auth::basic)
{
	cache->save_authorization(LIBCXX_NAMESPACE::http::responseimpl(code,
								       "Ok"),
				  uri,
				  LIBCXX_NAMESPACE::http::clientauthimpl
				  ::base::create_basic(uri, realm, "name",
						       name));
}

static std::string dumpauth(const LIBCXX_NAMESPACE::http::clientauth &auth)
{
	std::ostringstream o;

	const LIBCXX_NAMESPACE::http::authorization_map_t
		*const auths[] ={ &auth->proxy_authorizations,
				  &auth->www_authorizations };

	static const char * const names[]={"P","A"};

	for (size_t i=0; i<2; i++)
		for (const auto &a: *auths[i])
		{
			LIBCXX_NAMESPACE::http::requestimpl req;

			a.second->add_header(req, names[i]);

			o << ", "
			  << a.first << "="
			  << auth_tostring(a.second->scheme)
			  << "," << a.second->realm;
			for (const auto &h:req)
			{
				o << "," << h.first << ":"
				  << h.second.value();
			}
		}

	return o.str();
}

std::string authfailed(const clientauthcache &cache,
		       int code,
		       const LIBCXX_NAMESPACE::uriimpl &uri,
		       const std::string &realm,
		       const LIBCXX_NAMESPACE::http::clientauth &auth=
		       LIBCXX_NAMESPACE::http::clientauth::create())
{
	LIBCXX_NAMESPACE::http::responseimpl resp(code, "Ok");

	bool rc=cache->fail_authorization(uri, resp, auth,
					  LIBCXX_NAMESPACE::http::auth::basic,
					  realm,
					  LIBCXX_NAMESPACE::http::responseimpl
					  ::scheme_parameters_t());

	return LIBCXX_NAMESPACE::to_string(rc) + dumpauth(auth);
}

std::string authsearch(const clientauthcache &cache,
		       const LIBCXX_NAMESPACE::uriimpl &uri)
{
	LIBCXX_NAMESPACE::http::requestimpl req(LIBCXX_NAMESPACE::http::GET,
						uri);

	auto auth=LIBCXX_NAMESPACE::http::clientauth::create();

	cache->search_authorizations(req, auth);

	std::ostringstream o;

	o << LIBCXX_NAMESPACE::to_string(uri) << dumpauth(auth);

	for (const auto &h:req)
	{
		o << "," << h.first << ":"
		  << h.second.value();
	}

	return o.str();
}

void testclientauthcache()
{
	auto cache=LIBCXX_NAMESPACE::ref<testclientauthcacheObj>::create();

	authsave(cache, proxy_auth_code, "http://example.com/cgi-bin/dump",
		 "proxyrealm", "proxy1");

	authsave(cache, www_auth_code, "http://example.com/cgi-bin/dump",
		 "originrealm1", "origin1");

	authsave(cache, www_auth_code, "http://public.com/cgi-bin/dump",
		 "originrealm2", "origin2");

	std::cout << "AUTH1: " << cache->to_string() << std::endl;
	std::cout << "SEARCH: "
		  << authsearch(cache, "http://example.com") << std::endl;
	std::cout << "SEARCH: "
		  << authsearch(cache, "http://example.com/cgi-bin/dump")
		  << std::endl;
	std::cout << "SEARCH: "
		  << authsearch(cache, "http://example.com/cgi-bin/dump/sub")
		  << std::endl;
	std::cout << "SEARCH: "
		  << authsearch(cache, "http://public.com/cgi-bin/dump")
		  << std::endl;

	std::cout << "FAILED(public.com, realm2): "
		  << authfailed(cache,
				www_auth_code,
				"http://public.com/cgi-bin/dump",
				"realm2") << std::endl;
	std::cout << "AUTH2: " << cache->to_string() << std::endl;

	auto auth=LIBCXX_NAMESPACE::http::clientauth::create();

	for (int i=3; i<5; ++i)
	{
		std::cout << "FAILED(public.com, originrealm2): "
			  << authfailed(cache,
					www_auth_code,
					"http://public.com/cgi-bin/dump",
					"originrealm2", auth) << std::endl;
		std::cout << "AUTH" << i << ": " << cache->to_string()
			  << std::endl;
	}
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::option::list
			options(LIBCXX_NAMESPACE::option::list::create());

		options->addDefaultOptions();

		LIBCXX_NAMESPACE::option::parser
			opt_parser(LIBCXX_NAMESPACE::option::parser::create());

		opt_parser->setOptions(options);
		int err=opt_parser->parseArgv(argc, argv);

		if (err == 0) err=opt_parser->validate();

		if (err)
		{
			if (err == LIBCXX_NAMESPACE::option::parser::base
			    ::err_builtin)
				exit(0);

			std::cerr << opt_parser->errmessage();
			std::cerr.flush();
			exit(1);
		}

		testuridecode();
		testclientauthcache();

		if (LIBCXX_NAMESPACE::http
		    ::auth_fromstring(LIBCXX_NAMESPACE::http
				      ::auth_tostring(LIBCXX_NAMESPACE::http
						      ::auth::digest))
		    != LIBCXX_NAMESPACE::http::auth::digest)
			throw EXCEPTION("from/to does not work");

	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testhier: "
			  << e << std::endl << e->backtrace;
		exit(1);
	}
	return (0);
}
