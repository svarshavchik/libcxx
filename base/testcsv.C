/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "csv.H"
#include "str2char.H"

#include <vector>
#include <iostream>
#include <iterator>

#define _ ,

static void test1csv(const char *str,
		     const char * const *values)
{
	std::string csvline;

	size_t i=0;

	for (i=0; values[i]; ++i)
		;

	LIBCXX_NAMESPACE::tocsv(&values[0], &values[i],
	      std::back_insert_iterator<std::string>(csvline));

	if (csvline != str)
		throw EXCEPTION(std::string("Expected [") + str + "], got ["
				+ csvline + "]");

	csvline += "\n";

	std::vector<std::string> parsedvals;

	std::string::const_iterator p=
		LIBCXX_NAMESPACE::fromcsv(csvline.begin(), csvline.end(),
					parsedvals);

	if (p == csvline.end() || *p++ != '\n' || p != csvline.end())
		throw EXCEPTION(std::string(str) + ": unparsing stopped unexpectedly");

	if (parsedvals.size() != i)
		throw EXCEPTION(std::string(str) + ": did not parse to the expected number of values");

	for (size_t j=0; j != i; ++j)
		if (parsedvals[j] != values[j])
			throw EXCEPTION(std::string(str) + ": "
					+ values[j] + ": did not extract to the expected value");
}

#define TESTCSV(str,values) { static const char * const vals[]={ values, 0}; \
			      test1csv(str, vals); }

static void testcsv()
{
	{
		static const char * const vals[]={0};

		test1csv("", vals);
	}

	TESTCSV("value","value");
	TESTCSV("value1,value2", "value1" _ "value2");
	TESTCSV("value,", "value" _ "");

	TESTCSV("\"val,ue\"", "val,ue");
	TESTCSV("\"val\"\"ue\"", "val\"ue");
	TESTCSV("a,\"val,ue\",b", "a" _ "val,ue" _ "b");
	TESTCSV("a,\"val\"\"ue\",b", "a" _ "val\"ue" _ "b");

	TESTCSV("\"val\nue\"", "val\nue");
	TESTCSV("a,\"val\nue\",b", "a" _ "val\nue" _ "b");

}

static void testiter()
{
	std::string stringvec[3];

	stringvec[0]="a";
	stringvec[2]="b";

	std::string res;

	for (LIBCXX_NAMESPACE::str2char_input_iter<std::string *>
		     b(&stringvec[0], &stringvec[3]),
		     e(&stringvec[3]); b != e; ++b)
	{
		res.push_back(*b);
	}

	if (res != "ab")
		throw EXCEPTION("str2char_input_iter sanity check failed");
}

int main(int argc, char **argv)
{
	try {
		testcsv();
		testiter();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "testcsv: "
			  << e << std::endl;
	}
	return (0);
}

