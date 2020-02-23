/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/glob.H"
#include "x/dir.H"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <iterator>

static void testglob()
{
	LIBCXX_NAMESPACE::dir::base::rmrf("testdir");

	mkdir("testdir", 0777);
	close(open("testdir/x", O_RDWR|O_CREAT|O_TRUNC, 0777));

	std::vector<std::string> buf;

	LIBCXX_NAMESPACE::glob::create()->expand("testdir/*")
		->expand("testdir/*")->get(std::back_insert_iterator
					   <std::vector<std::string> >(buf));

	if (buf.size() != 2 || buf[0] != "testdir/x"
	    || buf[1] != "testdir/x")
		throw EXCEPTION("expand(testdir/*) did not produce expected results");

	LIBCXX_NAMESPACE::glob::create()->expand("testdir/*")->get(buf);

	if (buf.size() != 3 || buf[2] != "testdir/x")
		throw EXCEPTION("get(std::vector<std::string> &) did not produce expected results");

	std::set<std::string> set;

	LIBCXX_NAMESPACE::glob::create()->expand("testdir/*")->get(set);
	LIBCXX_NAMESPACE::glob::create()->expand("testdir/*")->get(set);

	if (set.size() != 1)
		throw EXCEPTION("get(std::set<std::string> &) did not produce expected results");


	try {
		LIBCXX_NAMESPACE::glob::create()->expand("testdir/y*");
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		return;
	}
	throw EXCEPTION("glob() did not fail as expected");
}

int main(int argc, char **argv)
{
	alarm(30);

	try {
		testglob();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
		exit(1);
	}
	LIBCXX_NAMESPACE::dir::base::rmrf("testdir");
	return 0;
}
