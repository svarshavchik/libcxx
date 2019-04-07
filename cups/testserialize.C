/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/locale.H"
#include "x/exception.H"
#include "x/cups/destination.H"
#include <map>
#include <iostream>

#define DEBUG_FOR_SERIALIZE_COLLECTION					\
	const auto &value:						\
	std::map<std::string, option_values_t> {c->begin(), c->end()}

#include "job_impl.C"

void testserialize()
{
	auto c=LIBCXX_NAMESPACE::cups::collection::create();

	c->emplace("a", std::monostate{});
	c->emplace("b", std::unordered_map<std::string,
		   std::string>{ {"a", "b"}});
	c->emplace("c", std::unordered_map<int,
		   std::string>{ {1, "a"}, {2, "b"}});
	c->emplace("d", std::unordered_set<int>{3,4});
	c->emplace("e", std::unordered_set<bool>{false});
	c->emplace("f", std::vector<LIBCXX_NAMESPACE::cups::resolution>
		   { {LIBCXX_NAMESPACE::cups::resolution::per_inch,
					   300, 300}});

	c->emplace("g", std::vector<std::tuple<int, int>>
		   { {1, 2}, {3, 3}, {5, 6} });

	LIBCXX_NAMESPACE::cups::option_values_t opts;

	opts=std::vector<LIBCXX_NAMESPACE::cups::const_collection>{ c };

	auto s=LIBCXX_NAMESPACE::cups::serialize_values(opts);

	if (s != "{a=true b=a c=2,1 d=4,3 e=false f=300x300dpi g=1-2,3,5-6}")
		throw EXCEPTION("Wrong serialization results: "
				<< s);

	auto r=LIBCXX_NAMESPACE::cups::parse_range_string("1-2,,3,5-6");

	if (!r || *r != std::vector<std::tuple<int,int>>{{1,2},{3,3},{5,6}})
		throw EXCEPTION("parse_range_string test 1 failed");

	if (LIBCXX_NAMESPACE::cups::parse_range_string("2-1,3-3,5-6"))
		throw EXCEPTION("parse_range_string test 2 failed");

	if (LIBCXX_NAMESPACE::cups::parse_range_string("3,1-2,5-6"))
		throw EXCEPTION("parse_range_string test 3 failed");
}

int main(int argc, char *argv[])
{
	try {
		auto l=LIBCXX_NAMESPACE::locale::base::environment();

		l->global();
		testserialize();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
	}
	return 0;
}
