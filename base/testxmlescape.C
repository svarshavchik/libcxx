/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/xml/escape.H"

#include <iostream>
#include <cstdlib>
#include <vector>
	
static void testxmlescape()
{
	static const struct {
		const char *str;
		const char *escstr;
		bool qacflag;
	} teststrings[]={ {"foo\"\n", "foo\"\n", false},
			  {"foo\"\n", "foo&#x22;&#xa;", true},
			  {"\xFF", "\xFF", true},
	};

	for (size_t i=0; i<sizeof(teststrings)/sizeof(teststrings[0]); ++i)
	{
		if (LIBCXX_NAMESPACE::xml::escapestr(teststrings[i].str,
						   teststrings[i].qacflag)
		    != teststrings[i].escstr)
		{
			std::ostringstream o;

			o << "Encoding failure for string #" << i;

			throw EXCEPTION(o.str());
		}
	}
}

static void testxmlescapewide()
{
	static const int16_t shortbf[]={ 'a', '\n', 0x80, (int16_t)0xFFFF };

	std::string buf;

	LIBCXX_NAMESPACE::xml
		::escape(shortbf, &shortbf[sizeof(shortbf)/sizeof(shortbf[0])],
			 std::back_insert_iterator<std::string>(buf));

	if (buf != "a\n&#x80;&#xffff;")
		throw EXCEPTION("Encoding failure for wide string");

	buf.clear();

	LIBCXX_NAMESPACE::xml
		::escape(shortbf, &shortbf[sizeof(shortbf)/sizeof(shortbf[0])],
			 std::back_insert_iterator<std::string>(buf), true);

	if (buf != "a&#xa;&#x80;&#xffff;")
		throw EXCEPTION("Encoding failure for wide string (opt true)");
}

					 
int main(int argc, char **argv)
{
	try {
		testxmlescape();
		testxmlescapewide();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}

