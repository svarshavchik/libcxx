/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/pcre.H"
#include "x/exception.H"
#include "x/sysexception.H"
#include <iostream>
#include <type_traits>
#include <functional>

template<typename T, typename=void> struct pcre_match_no_temporary
	: std::false_type
{
};

template<typename T>
struct pcre_match_no_temporary<T,
	std::void_t<decltype(std::declval<LIBCXX_NAMESPACE::pcreObj &>()
			     .match(T{}))>> : std::true_type
{
};

static_assert(!pcre_match_no_temporary<std::string>::value,
	      "pcre->match() should not compile for an rvalue");


template<typename T, typename=void> struct pcre_match_all_no_temporary
	: std::false_type
{
};

template<typename T>
struct pcre_match_all_no_temporary<T,
	std::void_t<decltype(std::declval<LIBCXX_NAMESPACE::pcreObj &>()
			     .match_all(T{}))>> : std::true_type
{
};

static_assert(!pcre_match_all_no_temporary<std::string>::value,
	      "pcre->match_all() should not compile for an rvalue");

void testpcre(int argc, char **argv)
{
	auto pattern=LIBCXX_NAMESPACE::pcre::create("(.*) (.*)");

	auto patterns=pattern->match("abra cadabra");

	if (patterns.empty())
		throw EXCEPTION("Did not match! (1)");

	if (patterns.size() != 3 ||
	    patterns[0] != "abra cadabra" ||
	    patterns[1] != "abra" ||
	    patterns[2] != "cadabra")
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


void testpcre2(int argc, char **argv)
{
	auto pattern=LIBCXX_NAMESPACE::pcre::create("[^ ]+");

	auto patterns=pattern->match_all("abra cadabra");

	if (patterns.empty())
		throw EXCEPTION("Did not match! (2)");

	if (patterns != std::vector<std::vector<std::string_view>>{
			{ {"abra"} }, { {"cadabra"} } })
		throw EXCEPTION("Subpatterns (2)");
}

int main(int argc, char **argv)
{
	try {
		testpcre(argc, argv);
		testpcre2(argc, argv);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testpcre: "
			  << e << std::endl;
		exit(1);
	}
	return (0);
}
