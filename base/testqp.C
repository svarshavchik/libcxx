/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/qp.H"
#include "x/options.H"
#include "x/exception.H"
#include <vector>
#include <algorithm>
#include <iostream>

void testqp()
{
	static const struct {
		size_t w;
		bool crlf;
		const char *test;
		const char *expected;
	} tests[] = {
		{
			99,
			false,
			"a\rb\nc\t \t\nd  \n\n",
			"a=0Db\nc\t =09\nd =20\n\n",
		},
		{
			99,
			true,
			"e\rf\r\ng\t \t\r\nh  \r\n\r\n\n\r\n",
			"e=0Df\r\ng\t =09\r\nh =20\r\n\r\n=0A\r\n",
		},

		{
			4,
			false,
			"ab\ncdef\nghijk\nl=\nmn=\n",
			"ab\ncdef\nghij=\nk\nl=3D\nmn=\n=3D\n",
		},

		{
			4,
			true,
			"abcdef \r\n",
			"abcd=\r\nef=20\r\n",
		},

		{
			0,
			false,
			"a\nb",
			"a=0Ab",
		}
	};

	for (const auto &test:tests)
	{
		std::string s=test.test;
		std::string res;

		std::copy(s.begin(),
			  s.end(),
			  LIBCXX_NAMESPACE::qp_encoder
			  <std::back_insert_iterator<std::string> >
			  ( std::back_insert_iterator<std::string>(res),
			    test.w,
			    test.crlf)).eof();

		if (res != test.expected)
			throw EXCEPTION("Encoded [" + res
					+ "], but expected [" + test.expected
					+ "]");

		std::string orig;

		std::copy(res.begin(), res.end(),
			  LIBCXX_NAMESPACE::qp_decoder
			  <std::back_insert_iterator<std::string> >
			  ( std::back_insert_iterator<std::string>(orig)));

		if (orig != s)
			throw EXCEPTION("Decoded [" + orig
					+ "], but expected [" + s + "]");
	}
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
		testqp();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
