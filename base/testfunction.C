/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/functional.H"
#include "x/exception.H"
#include <iostream>

using namespace LIBCXX_NAMESPACE;

static char first_char(const function<int(const char *)> &func)
{
	return func("Test");
}

static int none(const function<int(void)> &func)
{
	return func();
}

static void noreturn(const function<void(int)>&f, int value)
{
	f(value);
}

static void testfunction()
{
	if (first_char(make_function<int(const char *)>([]
							(const char *p)
							{
								return *p;
							}))
	    != 'T')
		throw EXCEPTION("Test 1 failed");

	int index=1;
	auto lambda = [index]
		(const char *p)
		{
			return p[index];
		};

	if (first_char(make_function<int(const char *)>(lambda)) != 'e')
		throw EXCEPTION("Test 2 failed");


	if (none(make_function<int(void)>([&index]
		{
			return index;
		})) != 1)
		throw EXCEPTION("Test 3 failed");

	auto lambda2=[&index]
		(int n)
		{
			index=n;
		};

	noreturn(make_function<void(int)>(lambda2), 5);

	if (index != 5)
		throw EXCEPTION("Test 4 failed");
}

int main()
{
	try {
		testfunction();
	} catch (const exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}

	return 0;
}
