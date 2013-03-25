/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/xml/parser.H"
#include "x/xml/doc.H"
#include "x/exception.H"
#include "x/fd.H"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <unistd.h>
	
static LIBCXX_NAMESPACE::xml::doc parse(const std::string &str)
{
	return std::copy(str.begin(), str.end(),
			 LIBCXX_NAMESPACE::xml::parser::create("STRING"))
		.get()->done();
}

void test1(size_t bufsize)
{
	std::string filler(bufsize, 'X');

	{
		auto d=parse("<root><child name='value'>"
			     + filler +
			     "</child></root>");
	}

	bool flag=false;
	try {
		parse("<root><child />" + filler);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Caught expected exception:" << std::endl
			  << e;
		flag=true;
	}

	if (!flag)
		throw EXCEPTION("Did not catch the expected exception");

	{
		std::ofstream o("testxml.doc");

		o << "<root><child name='value'>" + filler + "</child></root>";

		o.close();

		auto d=LIBCXX_NAMESPACE::xml::doc::create("testxml.doc",
							  "nonet");
		unlink("testxml.doc");
	}
}

int main(int argc, char **argv)
{
	try {
		test1(100);
		test1(LIBCXX_NAMESPACE::fdbaseObj::get_buffer_size());
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}

