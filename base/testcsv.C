/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/csv.H"
#include "x/joiniterator.H"

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

	std::copy(LIBCXX_NAMESPACE::joiniterator<std::string *>
		  (stringvec, stringvec+3),
		  LIBCXX_NAMESPACE::joiniterator<std::string *>(),
		  std::back_insert_iterator<std::string>(res));

	if (res != "ab")
		throw EXCEPTION("joiniterator sanity check failed");

	res.clear();

	std::copy(LIBCXX_NAMESPACE::joiniterator<std::string *>
		  (stringvec, stringvec+3),
		  LIBCXX_NAMESPACE::joiniterator<std::string *>
		  (stringvec, stringvec+3).end(),
		  std::back_insert_iterator<std::string>(res));

	if (res != "ab")
		throw EXCEPTION("joiniterator sanity check 2 failed");

	res.clear();

	typedef std::reverse_iterator<LIBCXX_NAMESPACE::joiniterator
				      <std::string *> > rev_t;

	std::copy(rev_t(LIBCXX_NAMESPACE::joiniterator<std::string *>
			(stringvec, stringvec+3).end()),
		  rev_t(LIBCXX_NAMESPACE::joiniterator<std::string *>
			(stringvec, stringvec+3)),
		  std::back_insert_iterator<std::string>(res));
	if (res != "ba")
		throw EXCEPTION("joiniterator sanity check 3 failed");
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

