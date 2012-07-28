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
		std::cout << e << std::endl;
		return;
	}

	throw EXCEPTION("Exception handling failed");
}

int main(int argc, char **argv)
{
	try {
		testhttpresponseimpl();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}

