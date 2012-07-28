/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"

bool used_specialization=false;

#define LIBCXX_INTERNAL_DEBUG used_specialization=true;

#include "join.H"
#include "exception.H"
#include <list>
#include <iostream>

void testjoin()
{
	std::list<std::string> foobar = {"foo", "bar", "foobar"};

	used_specialization=false;
	if (LIBCXX_NAMESPACE::join(foobar.begin(), foobar.end(), ", ")
	    != "foo, bar, foobar")
		throw EXCEPTION("testjoin test 1 failed");

	if (!used_specialization)
		throw EXCEPTION("testjoin test 1 did not use specialized template");

	if (LIBCXX_NAMESPACE::join(foobar.begin(), foobar.end(),
				 std::string(", "))
	    != "foo, bar, foobar")
		throw EXCEPTION("testjoin test 2 failed");

	used_specialization=false;

	if (LIBCXX_NAMESPACE::join(foobar, ", ")
	    != "foo, bar, foobar")
		throw EXCEPTION("testjoin test 3 failed");

	if (!used_specialization)
		throw EXCEPTION("testjoin test 3 did not use specialized template");

	if (LIBCXX_NAMESPACE::join(foobar, std::string(", "))
	    != "foo, bar, foobar")
		throw EXCEPTION("testjoin test 4 failed");

}

void testwjoin()
{
	std::list<std::wstring> foobar = {L"foo", L"bar", L"foobar"};

	used_specialization=false;
	if (LIBCXX_NAMESPACE::join(foobar.begin(), foobar.end(), L", ")
	    != L"foo, bar, foobar")
		throw EXCEPTION("testwjoin test 1 failed");

	if (!used_specialization)
		throw EXCEPTION("testwjoin test 1 did not use specialized template");

	if (LIBCXX_NAMESPACE::join(foobar.begin(), foobar.end(),
				 std::wstring(L", "))
	    != L"foo, bar, foobar")
		throw EXCEPTION("testwjoin test 2 failed");

	used_specialization=false;

	if (LIBCXX_NAMESPACE::join(foobar, L", ")
	    != L"foo, bar, foobar")
		throw EXCEPTION("testwjoin test 3 failed");

	if (!used_specialization)
		throw EXCEPTION("testwjoin test 3 did not use specialized template");

	if (LIBCXX_NAMESPACE::join(foobar, std::wstring(L", "))
	    != L"foo, bar, foobar")
		throw EXCEPTION("testwjoin test 4 failed");

}

int main(int argc, char **argv)
{
	try {
		testjoin();
		testwjoin();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testjoin: "
			  << e << std::endl;
		exit(1);
	}
	return (0);
}

