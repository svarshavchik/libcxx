/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/options.H"
#include "x/mime/rfc2047.H"
#include "x/iconviofilter.H"
#include "x/exception.H"
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>

void testmimerfc2047()
{
	static const struct {
		const char *utf8;
		const char *rfc2047;
	} tests[] = {
		{
			"Hello world",
			"Hello world",
		},

		{
			"Hello w\xc3\xb3rld",
			"Hello =?UTF-8?B?d8Ozcmxk?=",
		},

		{
			"Hello w\xc3\xb3rld Hello",
			"Hello =?UTF-8?B?d8Ozcmxk?= Hello",
		},

		{
			"Hello w\xc3\xb3rld w\xc3\xb3rld",
			"Hello =?UTF-8?B?d8OzcmxkIHfDs3JsZA==?=",
		},

		{
			"Hello w\xc3\xb3rldworld",
			"Hello =?UTF-8?Q?w=C3=B3rldworld?=",
		},

		{
			"Hello w\xc3\xb3rldworld w\xc3\xb3rldworld",
			"Hello =?UTF-8?Q?w=C3=B3rldworld_w=C3=B3rldworld?=",
		},

		{
			"Hello w\xc3\xb3rldworldworld=w\xc3\xb3rldworld",
			"Hello =?UTF-8?Q?w=C3=B3rldworldworld=3Dw=C3=B3rldworld?=",
		},

		{
			"Hello w\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3"
			"\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3rld",
			"Hello =?UTF-8?B?d8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7Ny?= =?UTF-8?B?bGQ=?=",
		},

		{
			"Hello w\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3"
			"\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3rld",
			"Hello =?UTF-8?B?d8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7M=?= =?UTF-8?B?w7NybGQ=?=",
		},

		{
			"Heello w\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3"
			"\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3rld",
			"Heello =?UTF-8?B?d8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7M=?= =?UTF-8?B?w7NybGQ=?=",
		},

		{
			"Heeello w\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3"
			"\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3\xc3\xb3rld",
			"Heeello =?UTF-8?B?d8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7PDs8Ozw7M=?= =?UTF-8?B?w7NybGQ=?=",
		},

		{
			"Hello 012345678901234567890123456789012345678901234\xc3\xb3\xc3\xb3" "678901234567890",
			"Hello =?UTF-8?Q?012345678901234567890123456789012345678901234=C3=B3=C3=B3?= =?UTF-8?Q?678901234567890?=",
		},

		{
			"Hello 0123456789012345678901234567890123456789012345\xc3\xb3\xc3\xb3" "678901234567890",
			"Hello =?UTF-8?Q?0123456789012345678901234567890123456789012345=C3=B3?= =?UTF-8?Q?=C3=B3678901234567890?=",
		},

		{
			"Hello 01234567890123456789012345678901234567890123456\xc3\xb3\xc3\xb3" "78901234567890",
			"Hello =?UTF-8?Q?01234567890123456789012345678901234567890123456=C3=B3?= =?UTF-8?Q?=C3=B378901234567890?=",
		},

		{
			"Hello 012345678901234567890123456789012345678901234567\xc3\xb3\xc3\xb3" "8901234567890",
			"Hello =?UTF-8?Q?012345678901234567890123456789012345678901234567=C3=B3?= =?UTF-8?Q?=C3=B38901234567890?=",
		},

		{
			"Hello 0123456789012345678901234567890123456789012345678\xc3\xb3\xc3\xb3" "901234567890",
			"Hello =?UTF-8?Q?0123456789012345678901234567890123456789012345678=C3=B3?= =?UTF-8?Q?=C3=B3901234567890?=",
		},

		{
			"Hello 01234567890123456789012345678901234567890123456789\xc3\xb3\xc3\xb3" "01234567890",
			"Hello =?UTF-8?Q?01234567890123456789012345678901234567890123456789=C3=B3?= =?UTF-8?Q?=C3=B301234567890?=",
		},

		{
			"Hello 012345678901234567890123456789012345678901234567890\xc3\xb3\xc3\xb3" "1234567890",
			"Hello =?UTF-8?Q?012345678901234567890123456789012345678901234567890=C3=B3?= =?UTF-8?Q?=C3=B31234567890?=",
		},

		{
			"Hello 0123456789012345678901234567890123456789012345678901\xc3\xb3\xc3\xb3" "234567890",
			"Hello =?UTF-8?Q?0123456789012345678901234567890123456789012345678901?= =?UTF-8?Q?=C3=B3=C3=B3234567890?=",
		},
	};
	for (const auto &test:tests)
	{
		std::string utf8=test.utf8;

		std::string rfc2047=LIBCXX_NAMESPACE::mime
			::to_rfc2047_utf8(utf8);

		if (rfc2047 != test.rfc2047)
		{
			throw EXCEPTION("Got:      " + rfc2047 + "\n"
					"Expected: " + test.rfc2047);
		}

		std::string orig=
			LIBCXX_NAMESPACE::mime::from_rfc2047_as_utf8(rfc2047,
								     "UTF-8");

		if (orig != test.utf8)
			throw EXCEPTION("Got:      " + orig + "\n"
					"Expected: " + test.utf8);
	}

	std::string rfc2047=LIBCXX_NAMESPACE::mime
		::to_rfc2047_lang_utf8("EN", "Hello world");

	if (rfc2047 != "=?UTF-8*EN?Q?Hello_world?=")
		throw EXCEPTION("RFC 2231 extension does not work");

	rfc2047=LIBCXX_NAMESPACE::mime::from_rfc2047_as_utf8("=?UTF-8?Q?=invalid=?", "UTF-8");
	std::cout << "Just for kicks: " << rfc2047 << std::endl;

	auto pair=LIBCXX_NAMESPACE::mime::from_rfc2047_lang("=?iso-8859-1*en?q?O=A0k?=", "UTF-8");

	if (pair.first != "EN")
		throw EXCEPTION("Did not get EN");

	if (unicode::iconvert::convert(pair.second, unicode::utf_8)
	    != "O\xc2\xa0k")
		throw EXCEPTION("Did not transcode");

	std::cout <<
		LIBCXX_NAMESPACE::mime::from_rfc2047_as_utf8("foo\xA0" "bar",
							     "UTF-8")
		  << std::endl;
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::option::list
			options(LIBCXX_NAMESPACE::option::list::create());

		options->addDefaultOptions();

		LIBCXX_NAMESPACE::option::parser
			opt_parser(LIBCXX_NAMESPACE::option::parser::create());

		opt_parser->setOptions(options);
		int err=opt_parser->parseArgv(argc, argv);

		if (err == 0) err=opt_parser->validate();

		if (err)
		{
			if (err == LIBCXX_NAMESPACE::option::parser::base
			    ::err_builtin)
				exit(0);

			std::cerr << opt_parser->errmessage();
			std::cerr.flush();
			exit(1);
		}
		testmimerfc2047();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
