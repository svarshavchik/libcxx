/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/options.H"
#include "x/mime/bodystartiter.H"
#include "x/exception.H"
#include <iostream>
#include <sstream>
#include <iterator>

#define NEWLINE LIBCXX_NAMESPACE::mime::newline_start, '\n', LIBCXX_NAMESPACE::mime::newline_end

#define BODYSTART LIBCXX_NAMESPACE::mime::body_start

void testmimeiter()
{
	static struct {
		const std::vector<int> test_seq;
	} tests[] = {
		{
			{'a', NEWLINE, 'b', NEWLINE, NEWLINE, BODYSTART, 'c', NEWLINE, NEWLINE },
		},

		{
			{NEWLINE, BODYSTART, 'c', NEWLINE, NEWLINE },
		},

		{
			{BODYSTART},
		},
	};

	size_t test_num=0;

	for (const auto &test:tests)
	{
		++test_num;

		std::vector<int> expected=test.test_seq;

		expected.push_back(LIBCXX_NAMESPACE::mime::eof);

		std::vector<int> actual;

		typedef std::back_insert_iterator<std::vector<int>> iter_t;

		auto test_iter=
			LIBCXX_NAMESPACE::mime::bodystart_iter<iter_t>
			::create(iter_t(actual));

		for (int n:expected)
		{
			if (n != LIBCXX_NAMESPACE::mime::body_start)
				*test_iter++=n;
		}

		if (expected != actual)
			throw EXCEPTION(({
						std::ostringstream o;

						o << "Test " << test_num
						  << "failed" << std::endl;
						o.str();
					}));
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
