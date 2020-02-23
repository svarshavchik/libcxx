/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"

bool used_specialization=false;

#define LIBCXX_INTERNAL_DEBUG used_specialization=true;

#include "x/join.H"
#include "x/exception.H"
#include "x/algorithm.H"
#include <list>
#include <iostream>
#include <utility>

template<typename T>
void testjoin()
{
	std::list<T> foobar = {"foo", "bar", "foobar"};

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

	if (LIBCXX_NAMESPACE::join(foobar, ", ")
	    != "foo, bar, foobar")
		throw EXCEPTION("testjoin test 3 failed");

	if (LIBCXX_NAMESPACE::join(foobar, std::string(", "))
	    != "foo, bar, foobar")
		throw EXCEPTION("testjoin test 4 failed");
}

template<typename T>
void testwjoin()
{
	std::list<T> foobar = {L"foo", L"bar", L"foobar"};

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

	if (LIBCXX_NAMESPACE::join(foobar, L", ")
	    != L"foo, bar, foobar")
		throw EXCEPTION("testwjoin test 3 failed");

	if (LIBCXX_NAMESPACE::join(foobar, std::wstring(L", "))
	    != L"foo, bar, foobar")
		throw EXCEPTION("testwjoin test 4 failed");

}

void testsortby()
{
	std::vector<size_t> order_by={6,3,5,4,2,0,1,7};
	std::vector<std::string> arr={"6","3","5","4","2","0","1", "7"};

	LIBCXX_NAMESPACE::sort_by(order_by,
				  [&]
				  (size_t a, size_t b)
				  {
					  std::swap(arr.at(a),
						    arr.at(b));
				  });

	if (arr != std::vector<std::string>{"0","1","2","3","4","5","6","7"})
		throw EXCEPTION("testsortby failed (1)");

	order_by={6,3,5,4,1,0,1,7};

	bool caught=false;

	try {

		LIBCXX_NAMESPACE::sort_by(order_by,
					  [&]
					  (size_t a, size_t b)
					  {
						  std::swap(arr.at(a),
							    arr.at(b));
					  });
	} catch (...)
	{
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("testsortby failed (2)");

	order_by={6,3,5,8,2,0,1,7};

	caught=false;

	try {

		LIBCXX_NAMESPACE::sort_by(order_by,
					  [&]
					  (size_t a, size_t b)
					  {
						  std::swap(arr.at(a),
							    arr.at(b));
					  });
	} catch (...)
	{
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("testsortby failed (3)");
}

int main(int argc, char **argv)
{
	try {
		testjoin<std::string>();
		testwjoin<std::wstring>();
		testjoin<const char *>();
		testsortby();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testjoin: "
			  << e << std::endl;
		exit(1);
	}
	return (0);
}
