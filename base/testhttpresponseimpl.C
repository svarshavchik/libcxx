/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/requestimpl.H"
#include "x/http/responseimpl.H"
#include "x/http/exception.H"
#include "x/http/cookie.H"
#include "x/getlinecrlf.H"
#include "x/chrcasecmp.H"
#include "x/headersimpl.H"
#include "x/ymdhms.H"
#include "x/options.H"
#include "x/http/cookiejar.H"
#include "x/property_properties.H"
#include <set>
#include <map>
#include <iostream>
#include <sstream>
#include <iterator>
#include "srcdir.h"

static void testhttpresponseimpl()
{
	try {
		std::string str=
			"get /images http/99.0\r\n"
			"Host: localhost\r\n"
			"\r\n";

		LIBCXX_NAMESPACE::http::requestimpl msg;

		if (msg.parse(str.begin(), str.end(), 0) != str.end())
			throw EXCEPTION("Did not parse the full message");
	} catch (LIBCXX_NAMESPACE::http::response_exception &e)
	{
		std::cout << "Expected exception: ";

		e.to_string(std::ostreambuf_iterator<char>(std::cout));

		std::cout << e.body << std::flush;
		return;
	}

	throw EXCEPTION("Exception handling failed");
}

void verifychallenge(const std::string &challenge, const std::string &expected)
{
	LIBCXX_NAMESPACE::http::responseimpl resp;

	resp.parse(challenge.begin(), challenge.end(), 1000);

	std::ostringstream o;

	resp.challenges([&]
			(LIBCXX_NAMESPACE::http::auth scheme,
			 const std::string &realm,
			 const LIBCXX_NAMESPACE::http::responseimpl
			 ::scheme_parameters_t &params)
			{
				o << LIBCXX_NAMESPACE::http::
					auth_tostring(scheme)
				  << "\n" << realm << "\n";

				for (auto &p:params)
				{
					o << p.first << "=" << p.second
					  << std::endl;
				}
			});

	std::string parsed=o.str();

	if (parsed != expected)
	{
		std::cerr << "Expected:" << std::endl << expected
			  << "Parsed:" << std::endl << parsed << std::flush;
		throw EXCEPTION("Failed");
	}
}

void testchallenges()
{
	verifychallenge("HTTP/1.0 401 Not Authorized\r\n"
			"WWW-Authenticate: Basic realm=\"Secret\", password=rosebud\r\n\r\n",
			"Basic\nSecret\npassword=rosebud\n");

	verifychallenge("HTTP/1.0 407 Not Authorized\r\n"
			"Proxy-Authenticate: Basic realm=\"Secret\", password=rosebud, Digest realm=first, password=opensesame\r\n"
			"Proxy-Authenticate: Basic realm=\"second\", password=abracadabra\r\n\r\n",
			"Basic\nSecret\npassword=rosebud\n"
			"Digest\nfirst\npassword=opensesame\n"
			"Basic\nsecond\npassword=abracadabra\n");
}

void testsetcurrentdate()
{
	LIBCXX_NAMESPACE::http::responseimpl resp;

	resp.getCurrentDate();

	resp.setCurrentDate();

	time_t now=LIBCXX_NAMESPACE::ymdhms();

	time_t now2=resp.getCurrentDate();

	if (now2 < now-30 || now2 > now+30)
		throw EXCEPTION("Something wrong with getCurrentDate");

	sleep(2);

	if ((time_t)resp.getCurrentDate() != now2)
		throw EXCEPTION("Something wrong with getCurrentDate (2)");

}

void cookies2string(const LIBCXX_NAMESPACE::uriimpl &request_uri,
		    const std::list<LIBCXX_NAMESPACE::http::cookie> &cookies,
		    const std::string &expected)
{
	std::ostringstream o;

	for (const auto &cookie:cookies)
	{
		std::string domain=cookie.getValidatedDomain(request_uri);
		std::string path=cookie.getValidatedPath(request_uri);

		if (domain.empty() || path.empty())
			continue;

		o << cookie.name << "=" << cookie.value << ","
		  << (cookie.expiration == (time_t)-1 ? "":(std::string)
		      LIBCXX_NAMESPACE::ymdhms(cookie.expiration,
					       LIBCXX_NAMESPACE::tzfile
					       ::base::utc()))
		  << "," << domain << "," << path
		  << "," << (cookie.secure ? 1:0) << ","
		  << (cookie.httponly ? 1:0) << "|";
	}

	std::string s=o.str();

	std::cout << s << std::endl;

	if (s != expected)
		throw EXCEPTION("Expected " + expected);
}

void testgetcookies()
{
	static const struct {
		const char *req;
		const char *exp;
	} tests[]={
		{"HTTP/1.0 200 Ok\r\n"
		 "Set-Cookie: name1=value1\r\n"
		 "Set-Cookie: name2=value2; domain=.example.com; path=/images\r\n"
		 "Set-Cookie: name3=value3; max-age=300\r\n"
		 "Set-Cookie: name4=value4; expires=Sun, 04 Nov 2012 13:00:00 GMT; secure; httponly\r\n"
		 "Date: Sun, 04 Nov 2012 12:00:00 GMT\r\n"
		 "\r\n",
		 "name1=value1,,subdomain.example.com,/images/subdir,0,0|name2=value2,,.example.com,/images,0,0|name3=value3,Sun, 04 Nov 2012 12:05:00 +0000,subdomain.example.com,/images/subdir,0,0|name4=value4,Sun, 04 Nov 2012 13:00:00 +0000,subdomain.example.com,/images/subdir,1,1|"},

		{"HTTP/1.0 200 Ok\r\n"
		 "Set-Cookie: test2name2=value2; domain=.example.com; path=/images/subdir/x\r\n"
		 "Date: Sun, 04 Nov 2012 12:00:00 GMT\r\n"
		 "\r\n",
		 "test2name2=value2,,.example.com,/images/subdir/x,0,0|"},
		{"HTTP/1.0 200 Ok\r\n"
		 "Set-Cookie: test3name2=value2; domain=.example.com; path=/path\r\n"
		 "Date: Sun, 04 Nov 2012 12:00:00 GMT\r\n"
		 "\r\n",
		 "test3name2=value2,,.example.com,/path,0,0|"},

		{"HTTP/1.0 200 Ok\r\n"
		 "Set-Cookie: test4name2=value2; domain=.example.com; path=/\r\n"
		 "Date: Sun, 04 Nov 2012 12:00:00 GMT\r\n"
		 "\r\n",
		 "test4name2=value2,,.example.com,/,0,0|"},

		{"HTTP/1.0 200 Ok\r\n"
		 "Set-Cookie: test5name2=value2; domain=example.com; path=/\r\n"
		 "Date: Sun, 04 Nov 2012 12:00:00 GMT\r\n"
		 "\r\n",
		 "test5name2=value2,,.example.com,/,0,0|"},

		{"HTTP/1.0 200 Ok\r\n"
		 "Set-Cookie: test6name2=value2; domain=.com; path=/\r\n"
		 "Date: Sun, 04 Nov 2012 12:00:00 GMT\r\n"
		 "\r\n",
		 ""},

		{"HTTP/1.0 200 Ok\r\n"
		 "Set-Cookie: test7name2=value2; domain=.domain.com; path=/\r\n"
		 "Date: Sun, 04 Nov 2012 12:00:00 GMT\r\n"
		 "\r\n",
		 ""},

		{"HTTP/1.0 200 Ok\r\n"
		 "Set-Cookie: test8name2=value2; domain=example.com; path=/\r\n"
		 "Date: Sun, 04 Nov 2012 12:00:00 GMT\r\n"
		 "\r\n",
		 "test8name2=value2,,.example.com,/,0,0|"},

		{"HTTP/1.0 200 Ok\r\n"
		 "Set-Cookie: test9name2=value2; domain=example.com; path=/foo\r\n"
		 "Date: Sun, 04 Nov 2012 12:00:00 GMT\r\n"
		 "\r\n",
		 "test9name2=value2,,.example.com,/foo,0,0|"},

		{"HTTP/1.0 200 Ok\r\n"
		 "Set-Cookie: test10name2=value2; domain=example.com; path=/foo/\r\n"
		 "Date: Sun, 04 Nov 2012 12:00:00 GMT\r\n"
		 "\r\n",
		 "test10name2=value2,,.example.com,/foo,0,0|"},

		{"HTTP/1.0 200 Ok\r\n"
		 "Set-Cookie: test11=\"value\xA0\"; domain=example.com; path=/foo/\r\n"
		 "Date: Sun, 04 Nov 2012 12:00:00 GMT\r\n"
		 "\r\n",
		 ""},
	};

	for (const auto &test:tests)
	{
		LIBCXX_NAMESPACE::http::responseimpl resp;

		std::string str(test.req);

		resp.parse(str.begin(), str.end(), 10000);

		std::list<LIBCXX_NAMESPACE::http::cookie> cookies;

		resp.getCookies(cookies);
		cookies2string("http://subdomain.example.com/images/subdir/",
			       cookies, test.exp);
	}

	{
		std::stringstream o;

		LIBCXX_NAMESPACE::http::responseimpl resp;

		LIBCXX_NAMESPACE::http::cookie c("name", "value");

		c.domain=".example.com";
		c.path="/images";
		c.expiration=LIBCXX_NAMESPACE
			::ymdhms(std::string("08-Aug-2012 17:30:31 GMT"),
				 LIBCXX_NAMESPACE::tzfile::base::utc(),
				 LIBCXX_NAMESPACE::locale::create("C"));
		c.secure=true;
		c.httponly=true;

		resp.addCookie(c);

		resp.to_string(std::ostreambuf_iterator<char>(o));

		std::string set_cookie;

		std::string line;

		while (!std::getline(o, line).eof())
		{
			if (line.substr(0, 11) == "Set-Cookie:")
				set_cookie=line;
		}

		if (set_cookie != "Set-Cookie: name=value; Expires=\"Wed, 08 Aug 2012 17:30:31 GMT\"; Domain=.example.com; Path=/images; Secure; HttpOnly\r")
			throw EXCEPTION("Unexpected Set-Cookie header: "
					+ set_cookie);
	}
}

void testallcookiestest(const LIBCXX_NAMESPACE::http::cookiejar &jar,

			const LIBCXX_NAMESPACE::uriimpl &uri,
			const std::string &set_cookie,

			const LIBCXX_NAMESPACE::uriimpl &request_uri,
			const std::string &expected_cookies,
			bool for_http=true)
{
	LIBCXX_NAMESPACE::http::responseimpl resp;

	std::string str("HTTP/1.0 200 Ok\r\nSet-Cookie: "
			+ set_cookie + "\r\n\r\n");

	resp.parse(str.begin(), str.end(), 10000);

	jar->store(uri, resp);

	std::list<std::pair<std::string, std::string> > cookies;

	jar->find(request_uri, cookies, for_http);

	std::ostringstream o;

	for (const auto &cookie:cookies)
	{
		o << cookie.first << "=" << cookie.second << "|";
	}

	if (o.str() != expected_cookies)
		throw EXCEPTION("Expected " + expected_cookies +
				", but got " + o.str());
}

void testallcookiestest(const LIBCXX_NAMESPACE::uriimpl &uri,
			const std::string &set_cookie,

			const LIBCXX_NAMESPACE::uriimpl &request_uri,
			const std::string &expected_cookies,
			bool for_http=true)
{
	testallcookiestest(LIBCXX_NAMESPACE::http::cookiejar::create(),
			   uri, set_cookie, request_uri, expected_cookies,
			   for_http);
}

void testallcookies()
{
	// Path

	testallcookiestest("http://subdomain.example.com/subdir/path",
			   "test1=value1",
			   "http://subdomain.example.com/subdir/path",
			   "test1=value1|");

	testallcookiestest("http://subdomain.example.com/subdir/path",
			   "test2=value2",
			   "http://subdomain.example.com/subdir/path/subpath",
			   "test2=value2|");

	testallcookiestest("http://subdomain.example.com/subdir/path",
			   "test3=value3",
			   "http://subdomain.example.com/subdir",
			   "test3=value3|");
	testallcookiestest("http://subdomain.example.com/subdir/path",
			   "test4=value4",
			   "http://subdomain.example.com/subdir/",
			   "test4=value4|");
	testallcookiestest("http://subdomain.example.com/subdir/path",
			   "test5=value5",
			   "http://subdomain.example.com/",
			   "");

	// Origin server only

	testallcookiestest("http://subdomain.example.com/subdir/path",
			   "test6=value6",
			   "http://deeper.subdomain.example.com/subdir/path",
			   "");

	testallcookiestest("http://Subdomain.example.com/subdir/path",
			   "test7=value7; domain=subdomain.example.com",
			   "http://subdomain.Example.com/subdir/path",
			   "test7=value7|");

	testallcookiestest("http://subdomain.example.com/subdir/path",
			   "test8=value8; domain=subdomain.example.Com",
			   "http://deeper.subdomain.example.com/subdir/path",
			   "test8=value8|");

	testallcookiestest("http://subdomain.example.com/subdir/path",
			   "test9=value9; domain=subdomain.example.com",
			   "http://example.com/subdir/path",
			   "");

	// Ignore leading period

	testallcookiestest("http://Subdomain.example.com/subdir/path",
			   "test10=value10; domain=.subdomain.example.com",
			   "http://subdomain.Example.com/subdir/path",
			   "test10=value10|");

	testallcookiestest("http://subdomain.example.com/subdir/path",
			   "test11=value11; domain=.subdomain.example.Com",
			   "http://deeper.subdomain.example.com/subdir/path",
			   "test11=value11|");

	testallcookiestest("http://subdomain.example.com/subdir/path",
			   "test12=value12; domain=.subdomain.example.com",
			   "http://example.com/subdir/path",
			   "");

	// Replacement

	{
		auto jar=LIBCXX_NAMESPACE::http::cookiejar::create();

		testallcookiestest(jar, "http://example.com/subdir/path",
				   "test12=value12; domain=.example.com",
				   "http://example.com/subdir/path",
				   "test12=value12|");

		testallcookiestest(jar, "http://example.com/subdir/path",
				   "test12=value12b; domain=.example.com",
				   "http://example.com/subdir/path",
				   "test12=value12b|");

		// #13 should appear in the list first, since it's path is
		// longest.

		testallcookiestest(jar, "http://example.com/subdir/path/sub",
				   "test13=value13; domain=.example.com",
				   "http://example.com/subdir/path/sub",
				   "test13=value13|test12=value12b|");
	}

	// Test path attribute

	testallcookiestest("http://subdomain.example.com/",
			   "test14=value14; domain=.subdomain.example.com; path=/subdir",
			   "http://subdomain.example.com/",
			   "");

	testallcookiestest("http://subdomain.example.com/",
			   "test15=value15; domain=.subdomain.example.com; path=/subdir",
			   "http://subdomain.example.com/subdir",
			   "test15=value15|");

	// Testing secure

	testallcookiestest("http://subdomain.example.com/",
			   "test16=value16; domain=.subdomain.example.com; secure",
			   "http://subdomain.example.com/",
			   "");

	testallcookiestest("http://subdomain.example.com/",
			   "test17=value17; domain=.subdomain.example.com; secure",
			   "HTTPS://subdomain.example.com/",
			   "test17=value17|");

	// Testing httponly

	testallcookiestest("http://subdomain.example.com/",
			   "test18=value18; domain=.subdomain.example.com; httponly",
			   "http://subdomain.example.com/",
			   "", false);

	testallcookiestest("http://subdomain.example.com/",
			   "test19=value19; domain=.subdomain.example.com; httponly",
			   "http://subdomain.example.com/",
			   "test19=value19|");

	// Testing expiration

	auto yesterday=
		LIBCXX_NAMESPACE::ymdhms(time(NULL)-24 * 60 * 60,
					 LIBCXX_NAMESPACE::tzfile::base::utc());

	auto tomorrow=
		LIBCXX_NAMESPACE::ymdhms(time(NULL)+24 * 60 * 60,
					 LIBCXX_NAMESPACE::tzfile::base::utc());

	testallcookiestest("http://subdomain.example.com/",
			   "test20=value20; domain=.subdomain.example.com; "
			   "Expires=\""
			   + LIBCXX_NAMESPACE::to_string
			   (yesterday.format("%a, %d %b %Y %H:%M:%S GMT"),
			    LIBCXX_NAMESPACE::locale::create("C"))
			   + "\"",
			   "http://subdomain.example.com/",
			   "");

	testallcookiestest("http://subdomain.example.com/",
			   "test22=value22; domain=.subdomain.example.com; "
			   "Expires=\""
			   + LIBCXX_NAMESPACE::to_string
			   (tomorrow.format("%a, %d %b %Y %H:%M:%S GMT"),
			    LIBCXX_NAMESPACE::locale::create("C"))
			   + "\"",
			   "http://subdomain.example.com/",
			   "test22=value22|");
}

void dostorecookiejar(const LIBCXX_NAMESPACE::http::cookiejar &jar,
		      const std::string &uri,
		      const std::string &cookie)
{
	LIBCXX_NAMESPACE::http::responseimpl resp;

	std::string str="HTTP/1.0 200 Ok\r\n"
		"Set-Cookie: " + cookie + "\r\n\r\n";

	resp.parse(str.begin(), str.end(), 10000);

	jar->store(uri, resp);
}

void storecookiejar(const LIBCXX_NAMESPACE::http::cookiejar &jar)
{
}

template<typename ...Args>
void storecookiejar(const LIBCXX_NAMESPACE::http::cookiejar &jar,
		    const std::string &uri,
		    const std::string &cookie,
		    Args && ...args)
{
	dostorecookiejar(jar, uri, cookie);
	storecookiejar(jar, std::forward<Args>(args)...);
}

void checkcookiejar(const LIBCXX_NAMESPACE::http::cookiejar &jar,
		    const std::string &expected)
{
	std::map<std::string, std::string> cookies;

	for (auto b=jar->begin(), e=jar->end(); b != e; )
	{
		auto c= *b++;

		cookies[c.name]=c.value;
	}

	std::ostringstream o;

	for (const auto &cookie:cookies)
	{
		o << cookie.first << "=" << cookie.second << "|";
	}

	std::string s=o.str();

	if (s != expected)
		throw EXCEPTION("testcookieiter: expected " + expected
				+ ", but got " + s);
}

void testcookieiter()
{
	auto jar=LIBCXX_NAMESPACE::http::cookiejar::create();

	checkcookiejar(jar, "");

	storecookiejar(jar,
		       "http://subdomain.example.com",
		       "name1=value1; path=\"/subdir\"; domain=.example.com",
		       "http://subdomain.example.com",
		       "name2=value2; path=\"/subdir\"; domain=.example.com",
		       "http://subdomain.example.com",
		       "name3=value3; path=\"/images/local/subdir\"; domain=.example.com",
		       "http://domain.com/images/subdir",
		       "name4=value4; domain=.domain.com");

	checkcookiejar(jar, "name1=value1|name2=value2|name3=value3|name4=value4|");

	storecookiejar(jar,
		       "http://subdomain.example.com",
		       "name0=value0; path=\"/subdir\"; domain=.example.com; expires=\"01 Jan 1960 00:00:00 GMT");

	checkcookiejar(jar, "name1=value1|name2=value2|name3=value3|name4=value4|");

	storecookiejar(jar,
		       "http://domain.com/images/subdir",
		       "name4=value4; domain=.domain.com; expires=\"01 Jan 1960 00:00:00 GMT");
	checkcookiejar(jar, "name1=value1|name2=value2|name3=value3|");

}

void testcookielimits()
{
	LIBCXX_NAMESPACE::property::load_property
		(LIBCXX_NAMESPACE_STR "::http::cookiejar::domainmax", "2",
		 true, true);

	LIBCXX_NAMESPACE::property::load_property
		(LIBCXX_NAMESPACE_STR "::http::cookiejar::cookiebytesmax",
		 "50", true, true);

	auto jar=LIBCXX_NAMESPACE::http::cookiejar::create();

	storecookiejar(jar,
		       "http://subdomain.example.com",
		       "limit2big=01234567890123456789012345678901234567890123456789; path=\"/subdir\"; domain=.example.com");
	// Cookie too large
	checkcookiejar(jar, "");

	// Max two cookies per domain
	storecookiejar(jar,
		       "http://subdomain.example.com",
		       "limit1=value1; path=\"/subdir/subdir2\"; domain=.example.com",
		       "http://subdomain.example.com",
		       "limit2=value2; path=\"/subdir\"; domain=.example.com",
		       "http://subdomain.example.com",
		       "limit3=value3; path=\"/subdir\"; domain=.example.com");
	checkcookiejar(jar, "limit2=value2|limit3=value3|");

	// Cookie in another domain
	storecookiejar(jar,
		       "http://subdomain1.domain.com",
		       "limit4=value4; path=\"/subdir\"; domain=.subdomain1.domain.com");
	checkcookiejar(jar, "limit2=value2|limit3=value3|limit4=value4|");

	// Max two cookies per domain.
	storecookiejar(jar,
		       "http://subdomain1.domain.com",
		       "limit5=value5; path=\"/subdir\"; domain=.subdomain1.domain.com");

	checkcookiejar(jar, "limit2=value2|limit3=value3|limit4=value4|limit5=value5|");

	// Refresh limit4
	storecookiejar(jar,
		       "http://subdomain1.domain.com",
		       "limit4=value4; path=\"/subdir\"; domain=.subdomain1.domain.com");

	checkcookiejar(jar, "limit2=value2|limit3=value3|limit4=value4|limit5=value5|");

	// limit5 should be the oldest cookie that gets purged

	storecookiejar(jar,
		       "http://subdomain2.domain.com",
		       "limit6=value6; path=\"/subdir\"; domain=.subdomain2.domain.com");
	checkcookiejar(jar, "limit2=value2|limit3=value3|limit4=value4|limit6=value6|");
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property::load_property
			(LIBCXX_NAMESPACE_STR "::http::effective_tld_names",
			 SRCDIR "/effective_tld_names.dat", true, true);

		LIBCXX_NAMESPACE::option::string_value
			uri(LIBCXX_NAMESPACE::option::string_value::create());

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

		testhttpresponseimpl();
		testchallenges();
		testgetcookies();
		testallcookies();
		testcookieiter();
		testcookielimits();
		testsetcurrentdate();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
