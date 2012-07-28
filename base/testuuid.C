/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "uuid.H"
#include "exception.H"
#include "serialize.H"
#include "deserialize.H"
#include "tostring.H"
#include <iostream>
#include <cstdlib>

static void testuuid()
{
	std::cout << LIBCXX_NAMESPACE::tostring(LIBCXX_NAMESPACE::uuid())
		  << std::endl;
	std::cout << LIBCXX_NAMESPACE::tostring(LIBCXX_NAMESPACE::uuid())
		  << std::endl;

	LIBCXX_NAMESPACE::uuid uuid1;

	std::string s=LIBCXX_NAMESPACE::tostring(uuid1);

	LIBCXX_NAMESPACE::uuid uuid2(s);

	std::string t=LIBCXX_NAMESPACE::tostring(uuid2);

	if (s != t)
		throw EXCEPTION("Failed to save and restore a UUID");

	std::vector<char> sbuf;

	sbuf.resize(LIBCXX_NAMESPACE::serialize::object(uuid2));
	LIBCXX_NAMESPACE::serialize::object(uuid2, sbuf.begin());

	LIBCXX_NAMESPACE::deserialize::object(uuid1, sbuf);

	std::cout << LIBCXX_NAMESPACE::tostring(uuid1) << std::endl;

}

int main(int argc, char **argv)
{
	alarm(10);

	try {
		testuuid();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}
