/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/pcre.H"
#include "x/exception.H"
#include "x/sysexception.H"

#include <iostream>

void testpcre(int argc, char **argv)
{
	auto pattern=LIBCXX_NAMESPACE::pcre::create("(.*) (.*)");

	if (!pattern->match("abra cadabra"))
		throw EXCEPTION("Did not match!");

	if (pattern->subpatterns.size() != 3 ||
	    pattern->subpatterns[0] != "abra cadabra" ||
	    pattern->subpatterns[1] != "abra" ||
	    pattern->subpatterns[2] != "cadabra")
		throw EXCEPTION("Subpatterns");

	bool caught=false;

	try {
		LIBCXX_NAMESPACE::pcre::create("(.*");
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Caught expected exception: " << e
			  << std::endl;
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("Did not catch expected exception");
}

int main(int argc, char **argv)
{
	try {
		testpcre(argc, argv);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testpcre: "
			  << e << std::endl;
		exit(1);
	}
	return (0);
}

