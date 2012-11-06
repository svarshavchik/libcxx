/*
** Copyright 2012 Double Precision, Inc.
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
#include <set>
#include <iostream>
#include <sstream>
#include <iterator>

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

		e.toString(std::ostreambuf_iterator<char>(std::cout));

		std::cout << e.body() << std::flush;
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

		resp.toString(std::ostreambuf_iterator<char>(o));

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

int main(int argc, char **argv)
{
	try {
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
		testsetcurrentdate();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}

