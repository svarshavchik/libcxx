/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/options.H"
#include "x/mime/newlineiter.H"
#include "x/exception.H"
#include <iostream>

void testmimeiter()
{
	static const struct {
		const char *seq;
		bool usecrlf;
		const char *expected;
	} tests[]={
		{
			"",
			false,
			""
		},
		{
			"a\nb\n",
			false,
			"a<\n>b<\n>"
		},
		{
			"a\nb",
			false,
			"a<\n>b<>"
		},
		{
			"a\n\r\n",
			true,
			"a\n<\r\n>"
		},
		{
			"a\r\r\n",
			true,
			"a\r<\r\n>"
		},
		{
			"a\r",
			true,
			"a\r<>"
		},
		{
			"a",
			true,
			"a<>"
		},
		{
			"a\r\nb\r\n",
			true,
			"a<\r\n>b<\r\n>"
		},
		{
			"a\n\n",
			false,
			"a<\n><\n>"
		},
		{
			"a\r\n\r\n",
			true,
			"a<\r\n><\r\n>"
		},

	};
	size_t test_number=0;

	for (const auto &test:tests)
	{
		std::ostringstream o;

		o << "Test " << ++test_number << ": ";

		std::vector<int> out;

		typedef LIBCXX_NAMESPACE::mime::newline_iter
			<std::back_insert_iterator<std::vector<int>>> iter_t;

		*std::copy(test.seq, test.seq+strlen(test.seq),
			   iter_t::create(std::back_insert_iterator<
					  std::vector<int>
					  >(out), test.usecrlf))++
			=LIBCXX_NAMESPACE::mime::eof;

		std::vector<int> out2;

		for (const char *p=test.expected; *p; ++p)
			out2.push_back(*p == '<' ? LIBCXX_NAMESPACE::mime::newline_start:
				       *p == '>' ? LIBCXX_NAMESPACE::mime::newline_end:*p);
		out2.push_back(LIBCXX_NAMESPACE::mime::eof);

		if (out != out2)
			throw EXCEPTION(o.str() << "mismatch");
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
		testmimeiter();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
