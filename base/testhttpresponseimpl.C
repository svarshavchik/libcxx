/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "http/requestimpl.H"
#include "http/responseimpl.H"
#include "http/exception.H"
#include "getlinecrlf.H"
#include "chrcasecmp.H"
#include "headersimpl.H"
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
			"WWW-Authenticate: basic realm=\"Secret\", password=rosebud\r\n\r\n",
			"basic\nSecret\npassword=rosebud\n");

	verifychallenge("HTTP/1.0 407 Not Authorized\r\n"
			"Proxy-Authenticate: basic realm=\"Secret\", password=rosebud, Digest realm=first, password=opensesame\r\n"
			"Proxy-Authenticate: basic realm=\"second\", password=abracadabra\r\n\r\n",
			"basic\nSecret\npassword=rosebud\n"
			"digest\nfirst\npassword=opensesame\n"
			"basic\nsecond\npassword=abracadabra\n");
}

int main(int argc, char **argv)
{
	try {
		testhttpresponseimpl();
		testchallenges();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}

