/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/serverauth.H"
#include "x/http/useragent.H"
#include "x/http/fdserver.H"
#include "x/fdlistener.H"
#include "x/netaddr.H"
#include "x/tostring.H"
#include "x/exception.H"
#include "x/options.H"

#include "http_auth_internal.C"
#include <iterator>
std::string dotestauthorization(const char *header)
{
	LIBCXX_NAMESPACE::http::requestimpl req;

	req.append(req.www_authorization, header);

	auto dummy=LIBCXX_NAMESPACE::http::serverauth
		::create("Test Realm",
			 std::set<LIBCXX_NAMESPACE::uriimpl>
			 ({
				 "/private",
					 "http://example.com/private"
					 }));

	LIBCXX_NAMESPACE::http::serverauthObj::authorization_request info;

	LIBCXX_NAMESPACE::http::auth res=
		dummy->authorization(req.www_authorization,
				     req, info);

	switch (res) {
	case LIBCXX_NAMESPACE::http::auth::basic:
		return "BASIC:" + info.username;
	case LIBCXX_NAMESPACE::http::auth::digest:
		return "DIGEST:" + info.username
			+ "," + LIBCXX_NAMESPACE::tostring(info.nonce
							   .time_since_epoch()
							   .count())
			+ "," + info.uri
			+ "," + LIBCXX_NAMESPACE::gcrypt::md::base::name
			(info.algorithm)
			+ "," + info.response
			+ (info.qop_auth.size() ?
			   "," + LIBCXX_NAMESPACE::tostring(info.nonce_count) +
			   "," + info.cnonce:"");
	}

	return "UNKNOWN";
}

void testauthorization()
{
	const struct {
		const char *value;
		const char *expected;
	} tests[] = {
		{"Unknown scheme=value", "UNKNOWN"},
		{"Basic a2FuZTpyb3NlYnVk", "BASIC:kane:rosebud"},
		{"Digest algorithm=md5, username=user, realm=\"Test Realm\", nonce=100, uri=/, response=0000", "DIGEST:user,100,/,MD5,0000"},
		{"Digest algorithm=md5, username=user, realm=\"Test Realm\", nonce=100, uri=/, response=0000, qop=auth, nc=000a, cnonce=c", "DIGEST:user,100,/,MD5,0000,10,c"},
		// Wrong realm
		{"Digest algorithm=md5, username=user, realm=\"Wrong Realm\", nonce=100, uri=/, response=0000", "UNKNOWN"},

		// nonce garbage
		{"Digest algorithm=md5, username=user, realm=\"Test Realm\", nonce=100x, uri=/, response=0000", "UNKNOWN"},
		{"Digest algorithm=md5, username=user, realm=\"Test Realm\", nonce=x, uri=/, response=0000", "UNKNOWN"},

		// cnonce garbage
		{"Digest algorithm=md5, username=user, realm=\"Test Realm\", nonce=100, uri=/, response=0000, qop=auth, nc=000az, cnonce=c", "UNKNOWN"},
	};

	for (const auto &t:tests)
	{
		std::string res=dotestauthorization(t.value);

		if (res != t.expected)
			throw EXCEPTION(std::string("Header [") + t.value
					+ "], expected: ["
					+ t.expected + "], but got ["
					+ res + "]");
	}
}

template<typename ComputeFunctor>
bool dotestdigestscheme(ComputeFunctor &&compute,
			std::list<LIBCXX_NAMESPACE::http::responseimpl
			::challenge_info> &challenges,
			LIBCXX_NAMESPACE::http::responseimpl
			::scheme_parameters_t
			&authentication_info,
			const char *reqpasswd,
			const char *authpasswd,
			int stale)
{
	challenges.clear();
	authentication_info.clear();

	auto dummy=LIBCXX_NAMESPACE::http::serverauth
		::create("Test Realm",
			 std::set<LIBCXX_NAMESPACE::uriimpl>
			 ({
				 "/private",
					 "http://example.com/private"
					 }));

	dummy->nonce_expiration=60;

	LIBCXX_NAMESPACE::http::requestimpl
		req(LIBCXX_NAMESPACE::http::GET,
		    "http://www.example.com/private");
	LIBCXX_NAMESPACE::http::serverauthObj::authorization_request info;

	typedef LIBCXX_NAMESPACE::http::serverauthObj::clock_t clock_t;

	info.username="citizenkane";
	info.nonce=clock_t::now()-std::chrono::duration_cast<clock_t::duration>
		(std::chrono::seconds(5+stale*120));

	info.nonce_count=1;
	info.cnonce=LIBCXX_NAMESPACE::tostring(LIBCXX_NAMESPACE::uuid());
	info.uri="/private";
	info.algorithm=GCRY_MD_MD5;

	std::string a1req=LIBCXX_NAMESPACE::http::serverauth::base
		::compute_a1(GCRY_MD_MD5,
			     "citizenkane",
			     reqpasswd,
			     "Test Realm");
	std::string a1auth=LIBCXX_NAMESPACE::http::serverauth::base
		::compute_a1(GCRY_MD_MD5,
			     "citizenkane",
			     authpasswd,
			     "Test Realm");

	std::string a2=
		std::string(LIBCXX_NAMESPACE::http::requestimpl
			    ::methodstr(req.getMethod()))
		+ ":" + info.uri;

	info.response=compute(a1req, a2, info, req);

	LIBCXX_NAMESPACE::http::responseimpl resp;

	if (dummy->doDigestScheme(req,
				  challenges,
				  resp.authentication_info,
				  resp,
				  a1auth, info)
	    != LIBCXX_NAMESPACE::http::auth::digest)
	{
		if (challenges.empty())
			throw EXCEPTION("Failed doDigestScheme() did not return challenges");

		resp.add_authenticate(challenges);

		resp.process(resp.www_authenticate,
			     []
			     (std::string::const_iterator b,
			      std::string::const_iterator e)
			     {
				     std::cout << std::string(b, e)
					       << std::endl;
			     });
		return false;
	}

	resp.toString(std::ostreambuf_iterator<char>(std::cout));

	if (!resp.get_authentication_info(resp.authentication_info,
					  authentication_info))
		throw EXCEPTION("Cannot parse the header I just added?");
	return true;
}

template<typename Functor>
void testdigestscheme(Functor &&functor)
{
	typedef std::list<LIBCXX_NAMESPACE::http::responseimpl
			  ::challenge_info> challenges_t;

	challenges_t challenges;
	LIBCXX_NAMESPACE::http::responseimpl::scheme_parameters_t
		authentication_info;

	if (!dotestdigestscheme(functor, challenges, authentication_info,
				"rosebud", "rosebud", 0))
		throw EXCEPTION("Successful test failed");

	if (dotestdigestscheme(functor, challenges, authentication_info,
			       "rosebud", "rosebud2", 1))
		throw EXCEPTION("Failed test succeeded");

	{
		auto p=challenges.front().params.equal_range("stale");

		if (p.first != p.second && p.first->second == "yes")
			throw EXCEPTION("Failed test returned a stale flag");
	}

	if (dotestdigestscheme(functor, challenges, authentication_info,
			       "rosebud", "rosebud", 1))
		throw EXCEPTION("Stale test succeeded");
	{
		auto p=challenges.front().params.equal_range("stale");

		if (p.first == p.second || p.first->second != "yes")
			throw EXCEPTION("Stale test did not return a stale flag");
	}
}

void testrfc2069()
{
	std::cout << "Testing rfc2069 digest scheme" << std::endl;

	testdigestscheme([]
			 (const std::string &a1,
			  const std::string &a2,
			  LIBCXX_NAMESPACE::http::serverauthObj
			  ::authorization_request &info,
			  const LIBCXX_NAMESPACE::http::requestimpl &req)
			 {
				 return LIBCXX_NAMESPACE::http
					 ::compute_rfc2069_digest
					 (info.algorithm, a1,
					  LIBCXX_NAMESPACE::tostring
					  (info.nonce.time_since_epoch()
					   .count()), a2);
			 });
}

void testrfc2617()
{
	std::cout << "Testing rfc2617 digest scheme" << std::endl;

	testdigestscheme([]
			 (const std::string &a1,
			  const std::string &a2,
			  LIBCXX_NAMESPACE::http::serverauthObj
			  ::authorization_request &info,
			  const LIBCXX_NAMESPACE::http::requestimpl &req)
			 {
				 info.qop_auth="auth";

				 return LIBCXX_NAMESPACE::http
					 ::compute_rfc2617_digest_auth
					 (info.algorithm, a1,
					  LIBCXX_NAMESPACE::tostring
					  (info.nonce.time_since_epoch()
					   .count()),
					  LIBCXX_NAMESPACE::http
					  ::nonce_count_to_hex(info
							       .nonce_count),
					  info.cnonce, info.qop_auth, a2);
			 });
}

// -------------------------------------------------------------------------

// Create a listener on some random port, for some server clss.

// Base class for a server that requires authentication. The callbacks define
// various hooks.

class auth_serverObj : public LIBCXX_NAMESPACE::http::fdserverimpl,
		       virtual public LIBCXX_NAMESPACE::obj {
	LOG_CLASS_SCOPE;
public:
	LIBCXX_NAMESPACE::http::serverauth auth;

	auth_serverObj(const LIBCXX_NAMESPACE::http::serverauth &authArg)
		: auth(authArg)
	{
	}

	~auth_serverObj() noexcept {}

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag) override
	{
		std::string username;
		std::list<LIBCXX_NAMESPACE::http::responseimpl::challenge_info>
			challenges;
		std::string authentication_info;

		LOG_DEBUG(({
					std::ostringstream o;
					o << "=== REQUEST ===" << std::endl;
					req.toString(std::ostreambuf_iterator
						     <char>(o));
					o << "=== END REQUEST ===";

					o.str();
				}));

		LIBCXX_NAMESPACE::http::responseimpl resp;

		if (authenticate(req, resp, username, challenges)
		    == LIBCXX_NAMESPACE::http::auth::unknown)
		{
			prethrow_unauthorized(challenges);

			LIBCXX_NAMESPACE::http::responseimpl
				::throw_unauthorized(challenges);
		}

		resp.append(LIBCXX_NAMESPACE::http::content_type_header::name,
			    "text/plain");

		std::string content=getcontent(username);

		send(resp, req, content.begin(), content.end());
	}

	virtual LIBCXX_NAMESPACE::http::
	auth authenticate(const LIBCXX_NAMESPACE::http::requestimpl &req,
			  LIBCXX_NAMESPACE::http::responseimpl &resp,
			  std::string &username,
			  std::list<LIBCXX_NAMESPACE::http::responseimpl
			  ::challenge_info> &challenges)=0;

	virtual void prethrow_unauthorized(std::list<LIBCXX_NAMESPACE::http
					   ::responseimpl::challenge_info>
					   &challenges)
	{
	}

	virtual std::string getcontent(const std::string &username)=0;
};

LOG_CLASS_INIT(auth_serverObj);

// A subclass of auth_server that implements digest authentication only.

class digestauth_serverObj : public auth_serverObj {
public:

	digestauth_serverObj(const LIBCXX_NAMESPACE::http::serverauth &authArg)
		: auth_serverObj(authArg)
	{
	}

	~digestauth_serverObj() noexcept {}

	LIBCXX_NAMESPACE::http::
	auth authenticate(const LIBCXX_NAMESPACE::http::requestimpl &req,
			  LIBCXX_NAMESPACE::http::responseimpl &resp,
			  std::string &username,
			  std::list<LIBCXX_NAMESPACE::http::responseimpl
			  ::challenge_info> &challenges) override
	{
		return auth->check_authentication
			(req,
			 resp,
			 username,
			 challenges,
			 []
			 (gcry_md_algos algorithm,
			  const std::string &realm,
			  const std::string &username,
			  const LIBCXX_NAMESPACE::uriimpl &uri)
			 {
				 if (username != "citizenkane")
					 return std::string();

				 return LIBCXX_NAMESPACE::http::serverauth
					 ::base::compute_a1(algorithm, username,
							    "rosebud",
							    realm);
			 });
	}
};

// A server instance factory for the listener

template<typename server_type>
class httpauth_factoryObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	LIBCXX_NAMESPACE::http::serverauth auth;
	
	httpauth_factoryObj()
		: auth(LIBCXX_NAMESPACE::http::serverauth
		       ::create("Test Realm",
				std::set<LIBCXX_NAMESPACE::uriimpl>
				({"/"})))
	{
	}

	~httpauth_factoryObj() noexcept {}

	LIBCXX_NAMESPACE::ref<server_type> create()
	{
		return LIBCXX_NAMESPACE::ref<server_type>::create(auth);
	}
};

// Create a listener that spawns off a new server, created by a factory
// for each accepted connection. The listener listens on some random port.
// Return a listener instanec, and the URL referencing the port that we're
// listening on.

template<typename factory_type>
static std::pair<LIBCXX_NAMESPACE::fdlistener, std::string>
createlistener(const factory_type &precreated_server,
	       const char *hostname="localhost")
{
	std::list<LIBCXX_NAMESPACE::fd> fdlist;

	LIBCXX_NAMESPACE::netaddr::create(hostname, 0)->bind(fdlist, true);

	int portnum=fdlist.front()->getsockname()->port();

	auto listener=LIBCXX_NAMESPACE::fdlistener::create(fdlist, 100);
	listener->start(LIBCXX_NAMESPACE::http::fdserver::create(),
			precreated_server);

	std::ostringstream o;

	o << "http://localhost:" << portnum;

	return std::make_pair(listener, o.str());
}

// An authenticated server subclass that takes notice when nonces change.

// A subclass wrapper for digestauth_serverObj
class httpauth_rfc2617_serverObj : public digestauth_serverObj {

public:
	typedef LIBCXX_NAMESPACE::http::serverauthObj::clock_t clock_t;

	clock_t::time_point lastnonce; // Last nonce received
	bool nonce_changed; // The last received request had a different nonce.

	httpauth_rfc2617_serverObj(const LIBCXX_NAMESPACE::http::serverauth
				   &authArg)
		: digestauth_serverObj(authArg)
	{
	}

	~httpauth_rfc2617_serverObj() noexcept
	{
	}

	// Received: wrapper. parse the authorization header ourselves,
	// and make a note of the nonce parameter.

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag) override
	{
		nonce_changed=false;

		LIBCXX_NAMESPACE::http::serverauthObj
			::authorization_request info;

		clock_t::time_point nonce;

		if (auth->authorization(req.www_authorization, req, info)
		    == LIBCXX_NAMESPACE::http::auth::digest)
		{
			nonce=info.nonce;
		}

		// Set nonce_changed only if the URI has a newnonce parameter.

		auto form=req.getURI().getForm();

		if (form->find("newnonce") != form->end())
		{
			if (nonce != lastnonce)
				nonce_changed=true;
		}
		lastnonce=nonce;

		digestauth_serverObj::received(req, bodyflag);
	}

	std::string getcontent(const std::string &username)
	{
		if (nonce_changed)
			return "NONCE CHANGED";
		return "Authenticated " + username;
	}
};

// Testsuite for nonce behavior.

template<typename server_type>
void testserverauth_rfc2069_or_2617()
{
	auto server=LIBCXX_NAMESPACE::ref<httpauth_factoryObj
					  <server_type>>::create();

	auto listener=createlistener(server);

	auto ua=LIBCXX_NAMESPACE::http::useragent::create();

	{
		auto resp=ua->request(LIBCXX_NAMESPACE::http::GET,
				      listener.second);

		resp->message.toString(std::ostreambuf_iterator<char>
				       (std::cout));
		if (resp->message.getStatusCode() !=
		    LIBCXX_NAMESPACE::http::responseimpl
		    ::www_authenticate_code ||
		    resp->challenges.size() != 1 ||
		    resp->challenges.begin()->second->scheme !=
		    LIBCXX_NAMESPACE::http::auth::digest)
		{
			throw EXCEPTION("Did not get the initial challenge");
		}

		ua->set_authorization(resp, resp->challenges.begin()->second,
				      "citizenkane", "notrosebud");
	}

	std::cout << "Sending wrong password" << std::endl;

	{
		auto resp=ua->request(LIBCXX_NAMESPACE::http::GET,
				      listener.second);

		resp->message.toString(std::ostreambuf_iterator<char>
				       (std::cout));
		if (resp->message.getStatusCode() !=
		    LIBCXX_NAMESPACE::http::responseimpl
		    ::www_authenticate_code ||
		    resp->challenges.size() != 1 ||
		    resp->challenges.begin()->second->scheme !=
		    LIBCXX_NAMESPACE::http::auth::digest)
		{
			throw EXCEPTION("Did not get the challenge");
		}

		ua->set_authorization(resp, resp->challenges.begin()->second,
				      "citizenkane", "rosebud");
	}

	size_t cnt=0;

	while (1)
	{
		std::cout << "Sending right password" << std::endl;

		auto resp=ua->request(LIBCXX_NAMESPACE::http::GET,
				      listener.second);

		resp->message.toString(std::ostreambuf_iterator<char>
				       (std::cout));
		if (resp->message.getStatusCodeClass() != 2)
		{
			throw EXCEPTION("Right password failed");
		}

		std::cout << std::string(resp->begin(), resp->end())
			  << std::endl;

		if (resp->message.find(resp->message.authentication_info)
		    == resp->message.end())
			break;

		if (++cnt > 10)
			throw EXCEPTION("Server keeps sending Authorization-Info");
	}

	cnt=0;
	while (1)
	{
		std::cout << "Waiting for another nonce" << std::endl;
		sleep(1);

		auto resp=ua->request(LIBCXX_NAMESPACE::http::GET,
				      listener.second);

		resp->message.toString(std::ostreambuf_iterator<char>
				       (std::cout));
		if (resp->message.getStatusCodeClass() != 2)
		{
			throw EXCEPTION("Right password failed");
		}

		std::cout << std::string(resp->begin(), resp->end())
			  << std::endl;

		if (resp->message.find(resp->message.authentication_info)
		    != resp->message.end())
			break;

		if (++cnt > 5)
			throw EXCEPTION("Server is not sending Authorization-Info");
	}

	std::cout << "Using the new nonce" << std::endl;

	auto resp=ua->request(LIBCXX_NAMESPACE::http::GET,
			      listener.second,
			      LIBCXX_NAMESPACE::http::form::parameters
			      ::create("newnonce", "1"));

	if (resp->message.getStatusCodeClass() != 2)
	{
		throw EXCEPTION("Newnonce request failed");
	}

	if (std::string(resp->begin(), resp->end()) != "NONCE CHANGED")
		throw EXCEPTION("Server did not see a new nonce");
}

void testserverauth_rfc2617()
{
	testserverauth_rfc2069_or_2617<httpauth_rfc2617_serverObj>();
}

// Implement RFC2069 authentication only. This is done by subclassing the
// stock RFC2617 server, with a hook that drops qop=auth from authenticatoin
// challenges. This results in the client sending RFC 2069 authentication
// requests.

class httpauth_rfc2069_serverObj : public httpauth_rfc2617_serverObj {

public:
	httpauth_rfc2069_serverObj(const LIBCXX_NAMESPACE::http::serverauth
				   &authArg)
		: httpauth_rfc2617_serverObj(authArg)
	{
	}

	~httpauth_rfc2069_serverObj() noexcept
	{
	}

	void prethrow_unauthorized(std::list<LIBCXX_NAMESPACE::http
				   ::responseimpl::challenge_info>
				   &challenges)
	{
		for (auto &challenge:challenges)
			challenge.params.erase("qop");
	}

};

void testserverauth_rfc2069()
{
	testserverauth_rfc2069_or_2617<httpauth_rfc2069_serverObj>();
}

// A subclass of auth_server that implements basic and digest auth.

class basicdigestauth_serverObj : public auth_serverObj {
public:

	basicdigestauth_serverObj(const LIBCXX_NAMESPACE::http::serverauth
				  &authArg)
		: auth_serverObj(authArg)
	{
	}

	~basicdigestauth_serverObj() noexcept {}

	std::string content;
	std::string digest_algorithm;

	LIBCXX_NAMESPACE::http::
	auth authenticate(const LIBCXX_NAMESPACE::http::requestimpl &req,
			  LIBCXX_NAMESPACE::http::responseimpl &resp,
			  std::string &username,
			  std::list<LIBCXX_NAMESPACE::http::responseimpl
			  ::challenge_info> &challenges) override
	{
		digest_algorithm.clear();

		auto ret=auth->check_authentication
			(req,
			 resp,
			 username,
			 challenges,
			 [this]
			 (gcry_md_algos algorithm,
			  const std::string &realm,
			  const std::string &username,
			  const LIBCXX_NAMESPACE::uriimpl &uri)
			 {
				 digest_algorithm=LIBCXX_NAMESPACE::gcrypt
				 ::md::base::name(algorithm) + " ";

				 if (username != "citizenkane")
					 return std::string();

				 return LIBCXX_NAMESPACE::http::serverauth
					 ::base::compute_a1(algorithm, username,
							    "rosebud",
							    realm);
			 },

			 []
			 (const std::string &usercolonpassword)
			 {
				 return usercolonpassword
					 == "citizenkane:rosebud";
			 });

		content=digest_algorithm
			+ LIBCXX_NAMESPACE::http::auth_tostring(ret);

		return ret;
	}

	std::string getcontent(const std::string &username)
	{
		return content + " " + username;
	}
};

class digest_over_basic_serverObj : public basicdigestauth_serverObj {
public:

	digest_over_basic_serverObj(const LIBCXX_NAMESPACE::http::serverauth
				    &authArg)
		: basicdigestauth_serverObj(authArg)
	{
	}

	~digest_over_basic_serverObj() noexcept
	{
	}

	LIBCXX_NAMESPACE::http::
	auth authenticate(const LIBCXX_NAMESPACE::http::requestimpl &req,
			  LIBCXX_NAMESPACE::http::responseimpl &resp,
			  std::string &username,
			  std::list<LIBCXX_NAMESPACE::http::responseimpl
			  ::challenge_info> &challenges) override
	{
		auto val=basicdigestauth_serverObj::authenticate(req, resp,
								 username,
								 challenges);

		if (val == LIBCXX_NAMESPACE::http::auth::basic)
			resp.throw_internal_server_error();
		return val;
	}
};

template<typename server_type>
std::string testbasicdigestauth()
{
	auto server=LIBCXX_NAMESPACE::ref<httpauth_factoryObj
					  <server_type>>::create();

	auto listener=createlistener(server);

	auto ua=LIBCXX_NAMESPACE::http::useragent::create();

	{
		auto resp=ua->request(LIBCXX_NAMESPACE::http::GET,
				      listener.second);

		resp->message.toString(std::ostreambuf_iterator<char>
				       (std::cout));
		if (resp->message.getStatusCode() !=
		    LIBCXX_NAMESPACE::http::responseimpl
		    ::www_authenticate_code ||
		    resp->challenges.size() != 1)
		{
			throw EXCEPTION("Did not get the initial challenge");
		}

		ua->set_authorization(resp, resp->challenges.begin()->second,
				      "citizenkane", "rosebud");
	}

	auto resp=ua->request(LIBCXX_NAMESPACE::http::GET, listener.second);

	resp->message.toString(std::ostreambuf_iterator<char>
			       (std::cout));
	if (resp->message.getStatusCodeClass() != 2)
	{
		throw EXCEPTION("Right password failed");
	}

	auto res=std::string(resp->begin(), resp->end());
	std::cout << res << std::endl;
	return res;
}

void testserverauth_digest_over_basic()
{
	if (testbasicdigestauth<basicdigestauth_serverObj>()
	    != "MD5 Digest citizenkane")
		throw EXCEPTION("Test failed");
}

// Force client to send basic auth by dropping digest challenge.

class digest_fallback_basic_serverObj : public basicdigestauth_serverObj {
public:

	digest_fallback_basic_serverObj(const LIBCXX_NAMESPACE::http::serverauth
				    &authArg)
		: basicdigestauth_serverObj(authArg)
	{
	}

	~digest_fallback_basic_serverObj() noexcept
	{
	}

	LIBCXX_NAMESPACE::http::
	auth authenticate(const LIBCXX_NAMESPACE::http::requestimpl &req,
			  LIBCXX_NAMESPACE::http::responseimpl &resp,
			  std::string &username,
			  std::list<LIBCXX_NAMESPACE::http::responseimpl
			  ::challenge_info> &challenges) override
	{
		auto val=basicdigestauth_serverObj::authenticate(req, resp,
								 username,
								 challenges);

		auto p=challenges.begin();

		while (p != challenges.end())
		{
			auto q=p;

			++p;

			if (q->scheme != LIBCXX_NAMESPACE::http::auth::basic)
				challenges.erase(q);
		}

		return val;
	}
};

void testserverauth_digest_fallback_basic()
{
	if (testbasicdigestauth<digest_fallback_basic_serverObj>()
	    != "Basic citizenkane:rosebud")
		throw EXCEPTION("Test failed");
}

void testserverauth_sha1_digest_preferred(const LIBCXX_NAMESPACE::property
					  ::propvalue &algorithms)
{
	LIBCXX_NAMESPACE::property
		::load_property(LIBCXX_NAMESPACE_WSTR
				L"::http::serverauth::algorithms", algorithms,
				true, true);

	if (testbasicdigestauth<basicdigestauth_serverObj>()
	    != "SHA1 Digest citizenkane")
		throw EXCEPTION("Test failed");
}

class testserverObj : public LIBCXX_NAMESPACE::http::fdserverimpl,
		      virtual public LIBCXX_NAMESPACE::obj {
public:
	LIBCXX_NAMESPACE::http::serverauth auth;

	testserverObj(const LIBCXX_NAMESPACE::http::serverauth &authArg)
		: auth(authArg)
	{
	}

	~testserverObj() noexcept {}

	void received(const LIBCXX_NAMESPACE::http::requestimpl &req,
		      bool bodyflag) override
	{
		discardbody();

		LIBCXX_NAMESPACE::http::responseimpl resp;

		std::string authusername;
		std::string authuri;
		std::string authalgorithm;

		std::list<LIBCXX_NAMESPACE::http::responseimpl
			  ::challenge_info> challenges;

		auto ret=auth->check_authentication
			(req, resp, authusername,
			 challenges,
			 [&]
			 (gcry_md_algos algorithm,
			  const std::string &realm,
			  const std::string &username,
			  const LIBCXX_NAMESPACE::uriimpl &uri)
			 {
				 if (username != "citizenkane")
					 return std::string();

				 uri.toString(std::back_insert_iterator
					      <std::string>(authuri));
				 authalgorithm=LIBCXX_NAMESPACE::gcrypt::md::base::name(algorithm);
				 return LIBCXX_NAMESPACE::http::serverauth
					 ::base::compute_a1(algorithm, username,
							    "rosebud",
							    realm);
			 },
			 [&]
			 (const std::string &usercolonpassword)
			 {
				 if (usercolonpassword != "citizenkane:rosebud")
					 return false;

				 authuri="/";
				 authalgorithm="basic";
				 return true;
			 });

		if (ret == LIBCXX_NAMESPACE::http::auth::unknown)
			LIBCXX_NAMESPACE::http::responseimpl
				::throw_unauthorized(challenges);

		resp.append(LIBCXX_NAMESPACE::http::content_type_header::name,
			    "text/plain");

		std::string content=({
				std::ostringstream o;

				o << "You've authenticated "
				  << authuri
				  << " using "
				  << authalgorithm
				  << " as "
				  << authusername;
				o.str();
			});

		send(resp, req, content.begin(), content.end());
	}
};

void runtestserver()
{
	auto server=LIBCXX_NAMESPACE::ref<httpauth_factoryObj
					  <testserverObj>>::create();
	auto listener=createlistener(server, "");

	std::cout << "Listening on port " << listener.second
		  << ", press ENTER to stop: " << std::flush;

	std::string dummy;

	std::getline(std::cin, dummy);
}

void testdomainspace()
{
	auto auth=LIBCXX_NAMESPACE::http::serverauth
		::create("Test Realm", "/private", "/cgi",
			 "http://example.com",
			 "https://example.com");
	std::set<LIBCXX_NAMESPACE::uriimpl>
		expected_results={ "/private", "/cgi",
				   "http://example.com/private",
				   "http://example.com/cgi",
				   "https://example.com/private",
				   "https://example.com/cgi" };

	if (auth->domain != expected_results)
		throw EXCEPTION("testdomainspace() failed");
}

void make_sure_check_proxy_authentication_compiles
(LIBCXX_NAMESPACE::http::serverauth auth,
 const LIBCXX_NAMESPACE::http::requestimpl &req,
 LIBCXX_NAMESPACE::http::responseimpl &resp,
 std::string &username,
 std::list<LIBCXX_NAMESPACE::http::responseimpl ::challenge_info> &challenges)
{
	(void)auth->check_proxy_authentication
		(req,
		 resp,
		 username,
		 challenges,
		 []
		 (gcry_md_algos algorithm,
		  const std::string &realm,
		  const std::string &username,
		  const LIBCXX_NAMESPACE::uriimpl &uri)
		 {
			 return std::string();
		 });
};

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::option::list options(LIBCXX_NAMESPACE::option::list::create());

	LIBCXX_NAMESPACE::option::bool_value
		testserver(LIBCXX_NAMESPACE::option::bool_value::create());

	options->add(testserver, 0, L"server", 0,
		     L"Run a test server");

	options->addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser parser(LIBCXX_NAMESPACE::option::parser::create());

	parser->setOptions(options);

	if (parser->parseArgv(argc, argv) ||
	    parser->validate())
		exit(1);

	if (testserver->isSet())
		try {
			runtestserver();
			return 0;
		} catch (const LIBCXX_NAMESPACE::exception &e) {
			std::cout << e << std::endl;
			exit(1);
		}

	try {
		testauthorization();
		testrfc2069();
		testrfc2617();
		testdomainspace();
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
	}

	try {
		testserverauth_rfc2617();
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << "testserverauth_rfc2617: " << e << std::endl;
	}

	try {
		testserverauth_rfc2069();
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << "testserverauth_rfc2069: " << e << std::endl;
	}

	try {
		testserverauth_digest_over_basic();
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << "testserverauth_digest_over_basic: " << e << std::endl;
	}

	try {
		testserverauth_digest_fallback_basic();
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << "testserverauth_digest_fallback_basic: " << e << std::endl;
	}

	try {
		testserverauth_sha1_digest_preferred(L"MD5 SHA1");
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << "testserverauth_sha1_digest_preferred(MD5 SHA1): "
			  << e << std::endl;
	}

	try {
		testserverauth_sha1_digest_preferred(L"SHA1 MD5");
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << "testserverauth_sha1_digest_preferred(SHA1 MD5): "
			  << e << std::endl;
	}

	return 0;
}

