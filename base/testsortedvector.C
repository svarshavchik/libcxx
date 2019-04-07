/*
** Copyright 2015-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sorted_vector.H"
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

int main()
{
	try {
		testinsert1();
		testinsert2();
		testuniq();
		testremove();
		testadjust();
	} catch(const exception &e)
	{
		std::cout << e << std::endl;
		exit(1);
	}
	return (0);
}
