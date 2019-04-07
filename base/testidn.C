/*
** Copyright 2013-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/idn.H"

#include <iostream>
#include <stdlib.h>

static void testidn()
{
	std::string utf8_string=
		"www.r\xc3\xa4"
		"ksm\xc3\xb6"
		"rg\xc3\xa5"
		"s\xc2\xaa"
		".example";

	std::string s=LIBCXX_NAMESPACE::idn::to_ascii(utf8_string);

	if (s != "www.xn--rksmrgsa-0zap8p.example")
		throw EXCEPTION("to_ascii failed: " + s);

	std::string fa=LIBCXX_NAMESPACE::idn::from_ascii(s);

	if (fa != "www.r\xc3\xa4"
	    "ksm\xc3\xb6"
	    "rg\xc3\xa5"
	    "sa.example")
	{
		throw EXCEPTION("from_ascii failed: " + fa);
	}
}

int main(int argc, char **argv)
{
	try {
		testidn();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
