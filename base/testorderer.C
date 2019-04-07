/*
** Copyright 2013-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"

#include "x/orderedcache.H"
#include "x/exception.H"

#include <iostream>
#include <cstdlib>

void testorderer()
{
	auto orderer=[]
		(int key, const std::string &value)
		{
			return key;
		};

	typedef LIBCXX_NAMESPACE::ordered_cache_traits<int, std::string,
						decltype(orderer)> traits_t;

	LIBCXX_NAMESPACE::ordered_cache<traits_t>cache(3, orderer);

	cache.add(2, "4");
	cache.add(1, "2");
	cache.add(3, "5");
	cache.add(3, "6");

#define LINE2(l) # l
#define LINE(l) LINE2(l)
#define VERIFY_EXISTS(k, v) do {		\
		auto lookup=cache.find(k);	\
						\
		if (!lookup || *lookup != v)				\
			throw EXCEPTION("Did not find " #v " for " #k " at " LINE(__LINE__));	\
	} while (0)

#define VERIFY_GONE(k) do {						\
		if (cache.find(k)) throw EXCEPTION(#k " still exists at " LINE(__LINE__)); \
	} while (0)

	VERIFY_EXISTS(1, "2");
	VERIFY_EXISTS(2, "4");
	VERIFY_EXISTS(3, "6");

	cache.add(4, "8");

	VERIFY_GONE(1);
	VERIFY_EXISTS(2, "4");
	VERIFY_EXISTS(3, "6");
	VERIFY_EXISTS(4, "8");

	typedef LIBCXX_NAMESPACE::ordered_cache_traits<int, std::string> traits2_t;

	LIBCXX_NAMESPACE::ordered_cache<traits2_t> another_cache(3);

	another_cache.add(1, "X");
	another_cache.remove(1);

	{
		LIBCXX_NAMESPACE::ordered_cache<traits2_t, true> cache(2);

		cache.add(1, "A");
		cache.add(2, "B");

		cache.find(1);
		cache.add(3, "C");

		VERIFY_GONE(2);
		VERIFY_EXISTS(1, "A");
		VERIFY_EXISTS(3, "C");
	}

	{
		LIBCXX_NAMESPACE::ordered_cache<traits2_t, true> cache(2);

		cache.add(1, "A");
		cache.add(2, "B");

		cache.add(1, "AA");
		cache.add(3, "C");

		VERIFY_GONE(2);
		VERIFY_EXISTS(1, "AA");
		VERIFY_EXISTS(3, "C");
	}
}

int main(int argc, char **argv)
{
	try {
		testorderer();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testorderer: "
			  << e << std::endl;
		exit(1);
	}
	return (0);
}

