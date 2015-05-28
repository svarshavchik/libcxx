/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <string>
#include "x/fd.H"
#include "x/exception.H"

using namespace LIBCXX_NAMESPACE;

void testcombinepath()
{
	static const struct {
		const char *combine;
		const char *result;
	} tests[] = {
		{".", "."},
		{"subdir1", "subdir1"},
		{"subdir2/subdir3", "subdir1/subdir2/subdir3"},
		{"..", "subdir1/subdir2"},
		{"../..", "."},
		{"..", ".."},
		{"..", "../.."},
		{"/home/path/./", "/home/path"},
		{"/home", "/home"},
		{"..", "/"},
		{"..", "/"},
	};

	std::string dir=".";

	for (const auto &t:tests)
	{
		auto ndir=fd::base::combinepath(dir, t.combine);

		if (ndir != t.result)
			throw EXCEPTION("combine(\"" + dir +"\", \""
					+ t.combine + "\")=\""
					+ ndir + "\"");
		dir=ndir;
	}
}

int main()
{
	try {
		testcombinepath();
	} catch(const exception &e)
	{
		std::cout << e << std::endl;
		exit(1);
	}
	return (0);
}
