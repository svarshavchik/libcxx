/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ctype.H"
#include "x/exception.H"
#include "x/tostring.H"
#include "x/strftime.H"
#include "x/strtok.H"
#include "x/strsplit.H"
#include <iostream>
#include <sstream>
#include <iterator>

static void testctype()
{
	LIBCXX_NAMESPACE::ctype ctype(LIBCXX_NAMESPACE::locale::base::utf8());

	std::cout << ctype.toupper("Hello world") << std::endl;
	std::cout << ctype.toupper(std::string("Hello world")) << std::endl;
	std::cout << ctype.tolower("Hello world") << std::endl;
	std::cout << ctype.tolower(std::string("Hello world")) << std::endl;
}

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

	LIBCXX_NAMESPACE::ctype ct(LIBCXX_NAMESPACE::locale::base::utf8());

	ct.strtok_is("  ab \t cd", wordlist, std::ctype_base::space);
	std::cout << "strtok3: ";
	showwords(wordlist);
	std::cout << std::endl;
	wordlist.clear();

	ct.strtok_is(std::string("  ab \t cd"), wordlist,
		     std::ctype_base::space);
	std::cout << "strtok4: ";
	showwords(wordlist);
	std::cout << std::endl;
	wordlist.clear();

	ct.strtok_not("aa a  aaa a", wordlist, std::ctype_base::space);
	std::cout << "strtok5: [";
	showwords(wordlist);
	std::cout << "]" << std::endl;
	wordlist.clear();

	ct.strtok_not(std::string("aa a  aaa a"),
		     wordlist, std::ctype_base::space);
	std::cout << "strtok6: [";
	showwords(wordlist);
	std::cout << "]" << std::endl;
	wordlist.clear();

	ct.strtok_is("word1 word2  \"word3  word4\"\"word5 \" word6",
		     wordlist, std::ctype_base::space, '"');
	std::cout << "strtok7: [";
	showwords(wordlist);
	std::cout << "]" << std::endl;
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

int main(int argc, char *argv[])
{
	try {
		alarm(30);
		testctype();

		if (LIBCXX_NAMESPACE::strftime(1000000000, LIBCXX_NAMESPACE::tzfile::base::utc(), LIBCXX_NAMESPACE::locale::base::utf8())("%Y")
		    != "2001")
			throw EXCEPTION("What's up with strftime()?");


		teststrtok();
		teststrsplit();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
	return 0;
}
