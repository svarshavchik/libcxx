/*
** Copyright 2015-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sorted_vector.H"
#include "x/sorted_range.H"
#include "x/exception.H"

#include <iostream>
#include <cstdlib>
#include <string>
#include <algorithm>

using namespace LIBCXX_NAMESPACE;

void validate(const std::string &test,
	      const sorted_vector<char, char> &v,
	      const char *expected)
{
	std::string s;

	std::transform(std::begin(v),
		       std::end(v),
		       std::back_insert_iterator<std::string>(s),
		       []
		       (const auto &c)
		       {
			       return (char)c.first;
		       });

	if (s != expected)
		throw EXCEPTION(test + " failed: expected " + expected
				+ ", got " + s);
}

void testinsert1()
{
	struct {
		char c;
		bool ok;
		const char *st;
	} tests[]={
		{
			'D',
			true,
			"D",
		},
		{
			'N',
			true,
			"DN",
		},
		{
			'B',
			true,
			"BDN",
		},
		{
			'G',
			true,
			"BDGN",
		},
		{
			'O',
			true,
			"BDGNO",
		},
		{
			'B',
			false,
			"BDGNO",
		},
		{
			'G',
			false,
			"BDGNO",
		},
		{
			'O',
			false,
			"BDGNO",
		},
	};

	sorted_vector<char, char> sv;

	for (const auto &test:tests)
	{
		if (sv.insert_value(test.c, test.c) != test.ok)
			throw EXCEPTION("insert1() failed");

		validate("insert1", sv, test.st);
	}
}

void testinsert2()
{
	std::string s="BJN";

	struct {
		char pos;
		size_t cnt;
		const char *res;
	} tests[]={
		{
			'A',
			2,
			"BJN"
		}, {
			'B',
			2,
			"BDLP",
		}, {
			'C',
			1,
			"BCDKO",
		}, {
			'C',
			2,
			"BCELP",
		}, {
			'N',
			1,
			"BJNO",
		}, {
			'Q',
			2,
			"BJNQS"
		},
	};

	for (const auto &test:tests)
	{
		sorted_vector<char, char> sv;

		for (const auto &c:s)
			sv.insert_value(c, c);

		sv.insert_range(test.pos, test.cnt, test.pos);
		validate("insert2", sv, test.res);
	}
}

void testuniq()
{
	std::string s="1223224";

	sorted_vector<char, char> sv;

	char i='A';
	for (const auto &c:s)
		sv.insert_value(i++, c);

	sv.uniq();

	validate("uniq", sv, "ABDEG");
}

void testremove()
{
	std::string s="CGLO";

	struct {
		char remove_from;
		char remove_to;

		const char *res_remove;
		const char *res_shift;
	} tests[]= {
		{
			'A',
			'B',
			"CGLO",
			"CGLO",
		},
		{
			'A',
			'D',
			"GLO",
			"ADIL",
		},
		{
			'D',
			'E',
			"CGLO",
			"CDFKN",
		},
		{
			'D',
			'H',
			"CLO",
			"CDHK",
		},
		{
			'D',
			'L',
			"CLO",
			"CDG",
		},
		{
			'D',
			'M',
			"CO",
			"CDF",
		},
		{
			'N',
			'O',
			"CGLO",
			"CGLN",
		},
		{
			'N',
			'P',
			"CGL",
			"CGLN",
		},
	};

	for (const auto &test:tests)
	{
		sorted_vector<char, char> sv;

		for (const auto &c:s)
			sv.insert_value(c, c);

		auto sv2=sv;

		sv.remove(test.remove_from, test.remove_to);

		validate("remove", sv, test.res_remove);

		sv2.remove_range(test.remove_from, test.remove_to);

		validate("remove_range", sv2, test.res_shift);
	}
}

void testadjust()
{
	std::string s="CGLO";

	struct {
		char adjust;
		int diff;

		bool ok;
		const char *res;
	} tests[]= {
		{
			'A',
			1,
			true,
			"DHMP",
		}, {
			'C',
			1,
			true,
			"DHMP",
		}, {
			'D',
			1,
			true,
			"CHMP",
		}, {
			'D',
			-1,
			true,
			"CFKN",
		}, {
			'D',
			-10,
			false,
			"CGLO",
		}, {
			'Q',
			1,
			true,
			"CGLO",
		}
	};

	for (const auto &test:tests)
	{
		sorted_vector<char, char> sv;

		for (const auto &c:s)
			sv.insert_value(c, c);

		if (sv.adjust_keys(test.adjust, test.diff) != test.ok)
			throw EXCEPTION("adjust_keys unexpected return value");

		validate("adjust_keys", sv, test.res);
	}
}

void testvoidspecializatoin()
{
	sorted_vector<int, void> sv;

	sv.find(0);
	sv.remove(0, 1);
	sv.remove_range(0, 1);
	sv.duplicate(0);
	sv.insert_value(0);
	sv.insert_range(0, 1);
	sv.adjust_keys(0, 0);
}

struct testrange {
	int begin;
	int end;

	constexpr bool operator==(const testrange &o) const
	{
		return begin == o.begin &&
			end == o.end;
	}

	constexpr bool operator!=(const testrange &o) const
	{
		return !operator==(o);
	}

	constexpr auto operator<=>(const testrange &o) const
	{
		auto r=begin <=> o.begin;

		if (r == std::strong_ordering::equal)
			r=end <=> o.end;

		return r;
	}
};

void testaddrange()
{
	static const struct {

		std::vector<testrange> added_ranges;
		std::vector<testrange> final_ranges;
	} tests[] = {
		// Test 1
		{
			{
				{3, 4},
				{7, 7},
				{1, 2},
			},
			{
				{1, 2},
				{3, 4}
			}
		},
		// Test 2
		{
			{
				{3, 4},
				{1, 3},
			},
			{
				{1, 4},
			}
		},
		// Test 3
		{
			{
				{3, 5},
				{1, 4},
			},
			{
				{1, 5},
			}
		},
		// Test 4
		{
			{
				{3, 5},
				{1, 5},
			},
			{
				{1, 5},
			}
		},
		// Test 5
		{
			{
				{3, 5},
				{1, 6},
			},
			{
				{1, 6},
			}
		},
		// Test 6
		{
			{
				{1, 3},
				{8, 10},

				{4, 5},
			},
			{
				{1, 3},
				{4, 5},
				{8, 10},
			}
		},
		// Test 7
		{
			{
				{1, 3},
				{8, 10},

				{3, 5},
			},
			{
				{1, 5},
				{8, 10},
			}
		},
		// Test 8
		{
			{
				{1, 3},
				{8, 10},

				{2, 5},
			},
			{
				{1, 5},
				{8, 10},
			}
		},
		// Test 9
		{
			{
				{1, 3},
				{8, 10},

				{1, 5},
			},
			{
				{1, 5},
				{8, 10},
			}
		},
		// Test 10
		{
			{
				{1, 3},
				{8, 10},

				{7, 8},
			},
			{
				{1, 3},
				{7, 10},
			}
		},
		// Test 11
		{
			{
				{1, 3},
				{8, 10},

				{7, 9},
			},
			{
				{1, 3},
				{7, 10},
			}
		},
		// Test 12
		{
			{
				{1, 3},
				{8, 10},

				{7, 10},
			},
			{
				{1, 3},
				{7, 10},
			}
		},
		// Test 13
		{
			{
				{1, 3},
				{8, 10},

				{0, 10},
			},
			{
				{0, 10},
			}
		},
		// Test 14
		{
			{
				{0, 10},

				{0, 5},
			},
			{
				{0, 10},
			}
		},
		// Test 15
		{
			{
				{0, 10},

				{5, 10},
			},
			{
				{0, 10},
			}
		},
		// Test 16
		{
			{
				{0, 10},

				{5, 7},
			},
			{
				{0, 10},
			}
		},
	};

	int testnum=0;

	for (const auto &t:tests)
	{
		++testnum;
		sorted_range<testrange> r;

		for (const auto &v:t.added_ranges)
			r.add(v);

		std::vector<testrange> v{r.begin(), r.end()};

		if (v != t.final_ranges ||
		    (r == r && r < r && r != r))
			throw EXCEPTION("testaddrange test " << testnum
					<< " failed");
	}
}

void testremoverange()
{
	static const struct {
		std::vector<testrange> added_ranges;
		testrange remove;
		std::vector<testrange> final_ranges;
		testrange extract;
		std::vector<testrange> extracted;
	} tests[] = {
		// Test 1
		{
			{
				{5, 10},
				{15,20},
			},
			{8, 8},
			{
				{5, 10},
				{15,20},
			},
			{3, 4},
			{},
		},
		// Test 2
		{
			{
				{5, 10},
				{15,20},
			},
			{1, 1},
			{
				{5, 10},
				{15,20},
			},
			{3, 8},
			{
				{5, 8},
			},
		},
		// Test 3
		{
			{
				{5, 10},
				{15,20},
			},
			{1, 5},
			{
				{5, 10},
				{15,20},
			},
			{3, 12},
			{
				{5, 10},
			},

		},
		// Test 4
		{
			{
				{5, 10},
				{15,20},
			},
			{13, 14},
			{
				{5, 10},
				{15,20},
			},
			{7, 12},
			{
				{7, 10},
			},
		},
		// Test 5
		{
			{
				{5, 10},
				{15,20},
			},
			{1, 6},
			{
				{6, 10},
				{15,20},
			},
			{8, 18},
			{
				{8, 10},
				{15, 18},
			},
		},
		// Test 6
		{
			{
				{5, 10},
				{15,20},
			},
			{1, 10},
			{
				{15,20},
			},
			{15, 20},
			{
				{15, 20},
			},
		},
		// Test 7
		{
			{
				{5, 10},
				{15,20},
			},
			{8, 15},
			{
				{5, 8},
				{15,20},
			},
			{7, 15},
			{
				{7, 8},
			},
		},
		// Test 8
		{
			{
				{5, 10},
				{15,20},
			},
			{8, 16},
			{
				{5, 8},
				{16,20},
			},
			{1, 2},
			{
			},
		},
		// Test 9
		{
			{
				{5, 10},
				{15,20},
			},
			{8, 20},
			{
				{5, 8},
			},
			{8, 9},
			{
			},
		},
		// Test 10
		{
			{
				{5, 10},
				{15,20},
			},
			{8, 30},
			{
				{5, 8},
			},
			{7, 9},
			{
				{7, 8},
			},
		},
		// Test 11
		{
			{
				{5, 10},
				{15,20},
			},
			{18, 19},
			{
				{5, 10},
				{15, 18},
				{19, 20},
			},
			{10, 19},
			{
				{15, 18},
			},
		},
		// Test 12
		{
			{
				{5, 10},
				{15,20},
			},
			{13, 14},
			{
				{5, 10},
				{15, 20},
			},
			{7, 16},
			{
				{7, 10},
				{15, 16},
			},

		},
		// Test 13
		{
			{
				{5, 10},
				{15,20},
			},
			{20, 25},
			{
				{5, 10},
				{15, 20},
			},
			{3, 23},
			{
				{5, 10},
				{15, 20},
			}
		},
		// Test 14
		{
			{
				{5, 10},
				{15,20},
			},
			{21, 25},
			{
				{5, 10},
				{15, 20},
			},
			{1, 14},
			{
				{5, 10},
			},
		},
	};

	int testnum=0;

	for (const auto &t:tests)
	{
		++testnum;
		sorted_range<testrange> r;

		for (const auto &v:t.added_ranges)
			r.add(v);

		r.remove(t.remove);

		std::vector<testrange> v{r.begin(), r.end()};

		if (v != t.final_ranges)
			throw EXCEPTION("testremoverange test " << testnum
					<< " failed (1)");

		auto r_extracted=r.extract(t.extract);

		v=std::vector<testrange>{r_extracted.begin(),
			r_extracted.end()};

		if (v != t.extracted)
			throw EXCEPTION("testremoverange test " << testnum
					<< " failed (2)");
		r.shift(1);
		r.range();
	}
}

void testaddranges()
{
	sorted_range<testrange> a, b;

	a.add({1, 3});
	a.add({7, 9});

	b.add({5, 6});
	b.add({8, 10});

	a << b;

	a << std::move(b);

	std::vector<testrange> result{a.begin(), a.end()};

	if (result !=
	    std::vector<testrange>{ {1, 3}, {5, 6}, {7, 10} })
		throw EXCEPTION("testaddranges failed");
}

int main()
{
	try {
		testinsert1();
		testinsert2();
		testuniq();
		testremove();
		testadjust();
		testaddrange();
		testremoverange();
		testaddranges();
	} catch(const exception &e)
	{
		std::cout << e << std::endl;
		exit(1);
	}
	return (0);
}
