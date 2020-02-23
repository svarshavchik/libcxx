/*
** Copyright 2014-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/exception.H"
#include "x/to_string.H"
#include "x/strftime.H"
#include "x/strtok.H"
#include "x/strsplit.H"
#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>

static void showwords(const std::list<std::string> &s)
{
	const char *sep="";

	std::list<std::string>::const_iterator
		b=s.begin(),
		e=s.end();

	while (b != e)
	{
		std::cout << sep << *b++;
		sep=":";
	}
}


static void teststrtok()
{
	std::list<std::string> wordlist;

	LIBCXX_NAMESPACE::strtok_str("  ab \t cd", " \t", wordlist);
	std::cout << "strtok1: ";
	showwords(wordlist);
	std::cout << std::endl;
	wordlist.clear();

	LIBCXX_NAMESPACE::strtok_str(std::string("  ab \t cd"), " \t", wordlist);
	std::cout << "strtok2: ";
	showwords(wordlist);
	std::cout << std::endl;
	wordlist.clear();

	LIBCXX_NAMESPACE::strtok_str(std::string("  ab \"a b\" a\"bc \"a \"\" b\" \"abc"), " \t", '"', wordlist);
	std::cout << "strtok3: ";
	showwords(wordlist);
	std::cout << std::endl;
	wordlist.clear();
}

void teststrsplit()
{
	static const struct {
		const char *str;
		const char *w0;
		const char *w1;
	} tests[]={
		{
			"abra 'abra cadabra'",
			"abra",
			"abra cadabra",
		},
		{
			"'abra cadabra'  abra",
			"abra cadabra",
			"abra",
		},
		{
			"abra 'abra cadabra",
			"abra",
			"abra cadabra",
		},
		{
			"abra 'abra cadabra''",
			"abra",
			"abra cadabra'",
		},
		{
			"'abra''cadabra''' abra",
			"abra'cadabra'",
			"abra",
		},
		{
			"abra ''",
			"abra",
			"",
		},
	};

	for (size_t i=0; i<sizeof(tests)/sizeof(tests[0]); ++i)
	{
		std::vector<std::string> words;

		std::string test=tests[i].str;

		LIBCXX_NAMESPACE::strsplit(test.begin(), test.end(), words,
					   " ", '\'');

		if (words.size() != 2 ||
		    words[0] != tests[i].w0 ||
		    words[1] != tests[i].w1)
			throw EXCEPTION("strsplit failed");
	}
}

void testunicode()
{
	auto l=LIBCXX_NAMESPACE::locale::base::utf8();

	if (l->toupper("Здравствуйте") != "ЗДРАВСТВУЙТЕ")
		throw EXCEPTION("toupper failed");
	if (l->tolower("Здравствуйте") != "здравствуйте")
		throw EXCEPTION("tolower failed");


	auto ru=LIBCXX_NAMESPACE::locale::create("ru_RU.KOI8-R");

	std::string koi8r="\372\304\322\301\327\323\324\327\325\312\324\305";

	if (ru->tou32(koi8r) != U"Здравствуйте" ||
	    ru->fromu32(U"Здравствуйте") != koi8r ||
	    ru->toutf8(koi8r) != "Здравствуйте" ||
	    ru->fromutf8("Здравствуйте") != koi8r)
		throw EXCEPTION("iconvert test failed");
}

int main(int argc, char *argv[])
{
	try {
		alarm(30);

		auto c_locale=LIBCXX_NAMESPACE::locale::base::c();

		if (c_locale->charset() != "US-ASCII")
			throw EXCEPTION("C locale unknown charset");

		LIBCXX_NAMESPACE::locale::base::utf8()->global();
		if (LIBCXX_NAMESPACE::strftime(1000000000, LIBCXX_NAMESPACE::tzfile::base::utc(), LIBCXX_NAMESPACE::locale::base::utf8())("%Y")
		    != "2001")
			throw EXCEPTION("What's up with strftime()?");

		if (LIBCXX_NAMESPACE::to_string(2000,
						LIBCXX_NAMESPACE::locale::base
						::c())
		    != "2000")
			throw EXCEPTION("What's up with the C locale?");

		teststrtok();
		teststrsplit();
		testunicode();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
	return 0;
}
