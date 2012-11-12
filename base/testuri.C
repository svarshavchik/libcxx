/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/uriimpl.H"
#include "x/getlinecrlf.H"
#include "x/chrcasecmp.H"
#include "x/headersimpl.H"
#include "x/http/exception.H"
#include "x/base64.H"
#include <set>
#include <iostream>
#include <sstream>
#include <iterator>

void dumpuri(const LIBCXX_NAMESPACE::uriimpl &u)
{
	std::cout << "scheme: " << u.getScheme() << std::endl
		  << "authority: " << (u.getAuthority() ?
				       u.getAuthority().toString()
				       : std::string("(null)")) << std::endl
		  << "path: " << u.getPath() << std::endl
		  << "query: " << u.getQuery() << std::endl
		  << "fragment: " << u.getFragment() << std::endl
		  << "----" << std::endl;
}

void testuri(const std::string &str)
{
	LIBCXX_NAMESPACE::uriimpl u(str.begin(), str.end());

	dumpuri(u);
}

void testhostport(const std::string &str)
{
	std::pair<std::string, int> hostport=LIBCXX_NAMESPACE::uriimpl(str)
		.getHostPort();

	std::cout << "host: " << hostport.first << ", port " << hostport.second
		  << std::endl;
}

void testadd()
{
	static const char * const ptr[]={
		"g:h",
		"g",
		"./g",
		"g/",
		"/g",
		"//g",
		"?y",
		"g?y",
		"#s",
		"g#s",
		"g?y#s",
		";x",
		"g;x",
		"g;x?y#s",
		"",
		".",
		"./",
		"..",
		"../",
		"../g",
		"../..",
		"../../",
		"../../g",
		"../../../g",
		"../../../../g",
		"/./g",
		"/../g",
		"g.",
		".g",
		"g..",
		"..g",
		"./../g",
		"./g/.",
		"g/./h",
		"g/../h",
		"g;x=1/./y",
		"g;x=1/../y",

		"g?y/./x",
		"g?y/../x",
		"g#s/./x",
		"g#s/../x",

		"http:g",

		NULL};

	static const char base[]="http://a/b/c/d;p?q";

	for (size_t i=0; ptr[i]; ++i)
	{
		std::cout << base << " + " << ptr[i] << " = ";

		std::string bases(base), refs(ptr[i]);

		std::ostringstream o;

		(LIBCXX_NAMESPACE::uriimpl(bases.begin(), bases.end())
		 + LIBCXX_NAMESPACE::uriimpl(refs.begin(), refs.end()))
			.toString(std::ostreambuf_iterator<char>(o.rdbuf()));

		std::cout << o.str() << std::endl;
	}
}

static void except( void (*func)(),
			   const char *funcname)

{
	try {
		(*func)();
	} catch (LIBCXX_NAMESPACE::exception)
	{
		return;
	}

	throw EXCEPTION( std::string(funcname)
			 + " did not throw an exception");
}

static void invalid_auth()
{
	LIBCXX_NAMESPACE::uriimpl::authority_t("localhost/path");
}

static void testgetlinecrlf()
{
       static const char * const strings[]=
		{
			"",
			"foo",
			"foo\rbar",
			"foo\nbar",
			"foo\r"
		};

	static const char *sfix[2]={"\r\n", ""};

	for (size_t i=0; i<sizeof(strings)/sizeof(strings[0]); ++i)
	{

		for (size_t j=0; j<2; j++)
		{
			std::istringstream is(std::string(strings[i])+
					      sfix[j]);

			std::string test;

			LIBCXX_NAMESPACE::getlinecrlf(is, test);

			if (test != strings[i])
				throw EXCEPTION("getlinecrlf failed");

			if (i == 0 && j == 1 ? !is.fail():is.fail())
				throw EXCEPTION("getlinecrlf did not set stream state properly");
		}
	}
}

static void testchrcasecmp()
{
	static const struct testcase {
		const char *a;
		const char *b;
		bool result;
	} testcases[]={
		{"a", "A", false},
		{"A", "a", false},
		{"a", "b", true},
		{"a", "B", true},
		{"A", "b", true},
		{"A", "B", true},
		{"b", "a", false},
		{"B", "a", false},
		{"b", "A", false},
		{"B", "A", false},
		{"a", "aa", true},
		{"aa", "a", false}
	};

	LIBCXX_NAMESPACE::chrcasecmp::str_less cmp;

	for (size_t i=0; i<sizeof(testcases)/sizeof(testcases[0]); i++)
	{
		if (cmp(testcases[i].a, testcases[i].b) != testcases[i].result)
			throw EXCEPTION(std::string("chrcasecmp(\"") +
					testcases[i].a + "\", \"" +
					testcases[i].b + "\") is not " +
					(testcases[i].result ? "true":"false"));
	}
}

template<typename eol_type>
static void testheadersimpl()
{
	std::stringstream i;

	i << "a: b"	<< eol_type::eol_str
	  << "c:  d"	<< eol_type::eol_str
	  << " e "	<< eol_type::eol_str
	  << "a: f"	<< eol_type::eol_str
	  << "g:"	<< eol_type::eol_str
	  << "h: "	<< eol_type::eol_str
	  << eol_type::eol_str;

	i.seekg(0);

	typedef LIBCXX_NAMESPACE::headersimpl<eol_type> headers_t;

	headers_t headers;

	headers.parse(i, 0);

	{
		typename headers_t::const_iterator
			iter(headers.find("c"));

		if (iter == headers.end() ||
		    iter->second.value() != "d" + std::string(eol_type::eol_str)
		    + " e")
			throw EXCEPTION("headersimpl did not collapse headers properly");
	}

	if (headers.find("C") == headers.end())
		throw EXCEPTION("headersimpl is not case insensitive");

	{
		typename headers_t::const_iterator
			iter(headers.find("g"));

		if (iter == headers.end() || iter->second.value() != "")
			throw EXCEPTION("headersimpl did not collapse headers properly");
	}

	{
		typename headers_t::const_iterator
			iter(headers.find("h"));

		if (iter == headers.end() || iter->second.value() != "")
			throw EXCEPTION("headersimpl did not collapse headers properly");
	}

	{
		std::pair<typename headers_t::const_iterator,
			  typename headers_t::const_iterator>
			p(headers.equal_range("a"));

		if (p.first == p.second ||
		    p.first->second.value() != "b" ||
		    ++p.first == p.second ||
		    p.first->second.value() != "f" ||
		    ++p.first != p.second)
			throw EXCEPTION("headersimpl did not build the map correctly");
	}
}

template<typename Functor>
void testtokenizer(Functor &&functor)
{
	try
	{
		functor();
	} catch (const LIBCXX_NAMESPACE::http::response_exception &e)
	{
		std::stringstream s;

		e.toString(std::ostreambuf_iterator<char>(s));

		std::string line;

		while (!std::getline(s, line).eof())
		{
			if (line.substr(0, 5) == "Date:")
				continue;
			std::cout << line << std::endl;
		}

		std::cout << e.body << std::endl << std::flush;
	}
}

void testtokenizer()
{
	testtokenizer([]
		      {
			      LIBCXX_NAMESPACE::http::responseimpl
				      ::throw_unauthorized(LIBCXX_NAMESPACE
							   ::http::auth
							   ::basic,
							   "simplerealm",
							   LIBCXX_NAMESPACE::http::responseimpl::auth_param({"mode", false}),
							   "test");
		      });

	testtokenizer([]
		      {
			      LIBCXX_NAMESPACE::http::responseimpl
				      ::throw_unauthorized(LIBCXX_NAMESPACE
							   ::http::httpver_t
							   ::http11,
							   LIBCXX_NAMESPACE
							   ::http::auth
							   ::basic,
							   "simplerealm2",
							   LIBCXX_NAMESPACE::http::responseimpl::auth_param({"mode2", false}),
							   "test2",
							   LIBCXX_NAMESPACE
							   ::http::auth
							   ::basic,
							   "simplerealm3");
		      });

	testtokenizer([]
		      {
			      LIBCXX_NAMESPACE::http::responseimpl
				      ::throw_proxy_authentication_required
				      (LIBCXX_NAMESPACE
				       ::http::auth
				       ::basic,
				       "simplerealm4");
		      });

	testtokenizer([]
		      {
			      LIBCXX_NAMESPACE::http::responseimpl
				      ::throw_unauthorized(LIBCXX_NAMESPACE
							   ::http::auth
							   ::basic,
							   "prepackaged realm5");
		      });

	testtokenizer([]
		      {
			      LIBCXX_NAMESPACE::http::responseimpl
				      ::throw_unauthorized(LIBCXX_NAMESPACE
							   ::http::auth
							   ::basic,
							   "prepackaged realm6",
							   LIBCXX_NAMESPACE
							   ::http::auth
							   ::basic,
							   "realm7",
							   LIBCXX_NAMESPACE::http::responseimpl::auth_param({"param7", false}),
							   "value7",
							   LIBCXX_NAMESPACE
							   ::http::auth
							   ::basic,
							   "realm8");
		      });

	testtokenizer([]
		      {
			      std::list<LIBCXX_NAMESPACE::http::
					responseimpl::challenge_info> list;

			      list.push_back(LIBCXX_NAMESPACE::http::
					     responseimpl::challenge_info
					     (LIBCXX_NAMESPACE
					      ::http::auth::basic,
					      "7", {{"qop", "auth"}}));

			      list.push_back(LIBCXX_NAMESPACE::http::
					     responseimpl::challenge_info
					     (LIBCXX_NAMESPACE
					      ::http::auth::digest,
					      "7", {{"qop", "auth-int"}}));

			      LIBCXX_NAMESPACE::http::responseimpl
				      ::throw_unauthorized(list);
		      });

	std::cout << "Valid base64 encode: ";
	std::string up="user:password";

	LIBCXX_NAMESPACE::base64<>::encode(up.begin(), up.end(),
					   std::ostreambuf_iterator<char>
					   (std::cout));
}

int main(int argc, char **argv)
{
	try {
		if (LIBCXX_NAMESPACE::uriimpl("http://localhost")
		    .compare(LIBCXX_NAMESPACE::uriimpl("HTTP://localhost")))
			throw EXCEPTION("URI scheme is not case insensitive");

		if (LIBCXX_NAMESPACE::uriimpl("http://user@localhost")
		    .compare(LIBCXX_NAMESPACE::uriimpl("http://user@LOCALHOST")))
			throw EXCEPTION("URI authority host is not case insensitive");
		if (LIBCXX_NAMESPACE::uriimpl("http://user@localhost")
		    .compare(LIBCXX_NAMESPACE::uriimpl("http://USER@localhost"))
		    == 0)
			throw EXCEPTION("URI authority userinfo is not case sensitive");

		testuri("http://uid:pw@host/path?query#fragment");
		testuri("http://uid:pw@host");
		testuri("http://uid:pw@host/");
		testuri("http://host:80/");
		testuri("http://uid@pw:host:80/path");
		testuri("http://[host]:80/path");
		testuri("http://uid:pw@[host]:80/path");
		testuri("foo/bar");
		testuri("none:blank");
		testuri("file:///etc/passwd");
		testhostport("HTTP://host");
		testhostport("http://user:pw@host:8000");

		{
			static const struct {
				const char *a;
				const char *b;
				bool isparent;
			} parent_test[]={
				{ "/foo", "/foo", true},
				{ "/foo", "/foo/bar", true},
				{ "/foo/bar", "/foo", false},

				{ "/foo", "http://example.com/foo", true},
				{ "/foo", "http://example.com/foo/bar", true},
				{ "/foo/bar", "http://example.com/foo", false},

				{ "/foo", "http://example.com/foo", true},
				{ "/foo", "http://example.com/foo/bar", true},
				{ "/foo/bar", "http://example.com/foo", false},

				{ "http://example.com/foo", "http://example.com/foo", true},
				{ "http://example.com/foo", "http://example.com/foo/bar", true},
				{ "http://example.com/foo/bar", "http://example.com/foo", false},

				{ "http://example.com/foo", "/foo", false},
				{ "http://example.com/foo", "/foo/bar", false},
				{ "http://example.com/foo/bar", "/foo", false},

				{ "http://example.com/foo", "http://domain.com/foo", false},
				{ "http://example.com/foo", "http://domain.com/foo/bar", false},
				{ "http://example.com/foo/bar", "http://domain.com/foo", false},

				{ "http://example.com/foo", "https://example.com/foo", false},
				{ "http://example.com/foo", "https://example.com/foo/bar", false},
				{ "http://example.com/foo/bar", "https://example.com/foo", false},
			};

			for (const auto &t:parent_test)
			{
				if ((LIBCXX_NAMESPACE::uriimpl(t.a)
				     << LIBCXX_NAMESPACE::uriimpl(t.b))
				    != t.isparent)
				{
					throw EXCEPTION(std::string(t.a)
							<< " << "
							<< t.b
							<< " should be "
							<< LIBCXX_NAMESPACE
							::tostring(t.isparent)
							<< ", but it's not");
				}
			}
		}

		testadd();

		{
			std::cout << "Testing authority" << std::endl;

			LIBCXX_NAMESPACE::uriimpl::authority_t
				a("user:pw@host:80");

			std::cout << "Authority: " << a.toString() << std::endl;
			std::cout << "userinfo: " << a.userinfo << std::endl;
			std::cout << "hostport: " << a.hostport << std::endl;

			LIBCXX_NAMESPACE::uriimpl::authority_t
				a2("user:pw@[host]:80");
			LIBCXX_NAMESPACE::uriimpl::authority_t
				a3("[host]:80");

			except(invalid_auth, "invalid_auth");
		}

		{
			std::set<LIBCXX_NAMESPACE::uriimpl> uriset;

			uriset.insert("http://localhost");
			uriset.insert("http://localhost");
			uriset.insert("http://localhost/foo");

			if (uriset.size() != 2)
				throw EXCEPTION("URI set should have two entries");
		}
		testgetlinecrlf();
		testchrcasecmp();
		testheadersimpl<LIBCXX_NAMESPACE::headersbase::crlf_endl>();
		testheadersimpl<LIBCXX_NAMESPACE::headersbase::lf_endl>();
		testtokenizer();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}

