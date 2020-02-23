/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/postponedcall.H"
#include "x/exception.H"
#include <string>

static size_t test1_cb(const std::string &a, const std::string &b)
{
	return a.size() + b.size();
}

class test1_cl {

public:

	int operator()(int a, int b)
	{
		return a+b;
	}
};

void test1()
{
	LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::postponedcallBaseObj<size_t> >
		call( LIBCXX_NAMESPACE::make_postponedcall(test1_cb, "a", "bb"));

	if (call->invoke() != 3)
		throw EXCEPTION("splat");

	auto call2=LIBCXX_NAMESPACE
		::make_postponedcall([](const std::string &a,
					const std::string &b)
				     {
					     return a+b;
				     }, "a", "bb");

	auto ret2=call2->invoke();

	std::string &ret2Ref=ret2;

	if (ret2Ref != "abb")
		throw EXCEPTION("splat 2");

	const test1_cl cl;

	auto call3=LIBCXX_NAMESPACE::make_postponedcall(cl, 2, 3);

	if (call3->invoke() != 5)
		throw EXCEPTION("splat 3");
}


int main()
{
	test1();
	return 0;
}
