/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/xml/escape.H"

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

std::string escapestr(const std::string_view &str,
		      bool escapeqac)
{
	std::string s;

	escape(str.begin(), str.end(),
	       std::back_insert_iterator{s}, escapeqac);

	return s;
}

std::u32string escapestr(const std::u32string_view &str,
		      bool escapeqac)
{
	std::u32string s;

	escape(str.begin(), str.end(),
	       std::back_insert_iterator{s}, escapeqac);

	return s;
}

std::string xpathescapestr(//! The original string
	const std::string_view &str
)
{
	return xpathescapestr(str.begin(), str.end());
}

template std::back_insert_iterator<std::string>
escape(std::string::const_iterator, std::string::const_iterator,
       std::back_insert_iterator<std::string>, bool);

template std::ostreambuf_iterator<char>
escape(std::string::const_iterator, std::string::const_iterator,
       std::ostreambuf_iterator<char>, bool);

template std::back_insert_iterator<std::string>
escape(std::string_view::const_iterator, std::string_view::const_iterator,
       std::back_insert_iterator<std::string>, bool);

template std::ostreambuf_iterator<char>
escape(std::string_view::const_iterator, std::string_view::const_iterator,
       std::ostreambuf_iterator<char>, bool);

template std::back_insert_iterator<std::u32string>
escape(std::vector<uint32_t>::const_iterator,
       std::vector<uint32_t>::const_iterator,
       std::back_insert_iterator<std::u32string>, bool);

template std::string
escapestr(std::string::const_iterator, std::string::const_iterator, bool);

template std::string
escapestr(std::string_view::const_iterator,
	  std::string_view::const_iterator, bool);

template std::u32string
escapestr(std::vector<char32_t>::const_iterator,
	  std::vector<char32_t>::const_iterator, bool);


template std::back_insert_iterator<std::string>
xpathescape(std::string::const_iterator, std::string::const_iterator,
	    std::back_insert_iterator<std::string>);

template std::ostreambuf_iterator<char>
xpathescape(std::string::const_iterator, std::string::const_iterator,
	    std::ostreambuf_iterator<char>);

template std::string
xpathescapestr(std::string::const_iterator, std::string::const_iterator);

template std::string
xpathescapestr(std::string_view::const_iterator,
	       std::string_view::const_iterator);

#if 0
{
#endif
}
