/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ref.H"
#include "x/obj.H"
#include "x/refptr_hash.H"
#include "x/exception.H"

#include <unordered_set>
#include <iostream>

using namespace LIBCXX_NAMESPACE;

template<typename ref_type>
void testhash()
{
	std::unordered_set<ref_type> us;

	ref_type a=ref_type::create(), b=ref_type::create(),
		c=ref_type::create();

	us.insert(a);
	us.insert(b);

	auto p=us.find(a);

	if (p == us.end() || *p != a)
		throw EXCEPTION("find failed (1)");

	p=us.find(b);

	if (p == us.end() || *p != b)
		throw EXCEPTION("find failed (1)");

	if (us.find(c) != us.end())
		throw EXCEPTION("find failed (3)");
}

int main()
{
	try {
		testhash<ref<obj>>();
		testhash<ptr<obj>>();
		testhash<const_ref<obj>>();
		testhash<const_ptr<obj>>();
	} catch (const exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}

	return 0;
}
