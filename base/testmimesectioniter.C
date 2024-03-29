/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/options.H"
#include "x/mime/sectioniter.H"
#include "x/mime/sectiondecoder.H"
#include "x/exception.H"
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>

inline int char2ints(char c)
{
	switch (c) {
	case '<':
		return LIBCXX_NAMESPACE::mime::newline_start;
	case '>':
		return LIBCXX_NAMESPACE::mime::newline_end;
	case '[':
		return LIBCXX_NAMESPACE::mime::section_start;
	case ']':
		return LIBCXX_NAMESPACE::mime::section_end;
	default:
		break;
	}
	return (unsigned char)c;
}

void dochar2ints(const std::string &s,
		 std::vector<int> &i)
{
	std::transform(s.begin(),
		       s.end(),
		       std::back_insert_iterator<std::vector<int>>(i),
		       char2ints);
}

inline char int2chars(int c)
{
	switch (c) {
	case LIBCXX_NAMESPACE::mime::newline_start:
		return '<';
	case LIBCXX_NAMESPACE::mime::newline_end:
		return '>';
	case LIBCXX_NAMESPACE::mime::section_start:
		return '[';
	case LIBCXX_NAMESPACE::mime::section_end:
		return ']';
	case '\n':
		return '~';
	case LIBCXX_NAMESPACE::mime::eof:
		return '=';
	default:
		break;
	}
	return c;
}

void doint2chars(const std::vector<int> &i,
		 std::string &s)
{
	std::transform(i.begin(),
		       i.end(),
		       std::back_insert_iterator<std::string>(s),
		       int2chars);
}

void testmimesectioniter()
{
	static const struct {
		const char *boundary;
		const char *test;
		const char *expected;
	} tests[] = {
		{
			"boundary",

			"--boundary<\n>"
			"a: b<\n>"
			"<\n>"
			"--boundaryignored<\n>"
			"c: d<\n>"
			"--boundary--<\n><\n>",

			"--boundary<\n>"
			"[a: b<\n>"
			"]<\n>"
			"--boundaryignored<\n>"
			"[c: d]<\n>"
			"--boundary--<\n><\n>",
		},

		{
			"boundary",

			"<\n>"
			"<\n>"
			"--boundary<\n>"
			"a: b<\n>"
			"--bou<\n>"
			"--boxes<\n>"
			"<\n><>",

			"<\n>"
			"<\n>"
			"--boundary<\n>"
			"[a: b<\n>"
			"--bou<\n>"
			"--boxes<\n>"
			"<\n><>]",
		},
	};

	for (const auto &test:tests)
	{
		std::vector<int> str, expected, result;

		dochar2ints(test.test, str);
		dochar2ints(test.expected, expected);

		*std::copy(str.begin(),
			   str.end(),
			   LIBCXX_NAMESPACE::mime::
			   section_iter<std::back_insert_iterator
			   <std::vector<int>>>
			   ::create(std::back_insert_iterator
				    <std::vector<int>>(result),
				    test.boundary))++
			=LIBCXX_NAMESPACE::mime::eof;

		expected.push_back(LIBCXX_NAMESPACE::mime::eof);

		if (expected != result)
		{
			std::string expected_str;
			std::string result_str;

			doint2chars(expected, expected_str);
			doint2chars(result, result_str);

			throw EXCEPTION("Expected: " + expected_str + "\n" +
					"     Got: " + result_str);
		}
	}
}

typedef std::back_insert_iterator<std::string> ins_iter;

typedef LIBCXX_NAMESPACE::mime::section_decoder decoder_iter;

typedef LIBCXX_NAMESPACE::mime::newline_iter<decoder_iter> newline_iter;

void decode(const std::string &string, const std::string &te,
	    const std::string &expected)
{
	std::string s;

	std::copy(string.begin(),
		  string.end(),
		  newline_iter::create(decoder_iter::create(te, ins_iter(s))));

	if (s != expected)
		throw EXCEPTION("Expected: [" + expected + "]\n"
				"Got:      [" + s + "]");
}

void testmimesectiondecoder()
{
	decode("Hello world\n", "8bit", "Hello world\n");
	decode("Hello=20world\n", "Quoted-Printable", "Hello world\n");
	decode("SGVsbG8gd29ybGQK", "base64", "Hello world\n");
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
		testmimesectioniter();
		testmimesectiondecoder();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
