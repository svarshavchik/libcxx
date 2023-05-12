/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_escape_H
#define x_xml_escape_H

#include <x/namespace.h>
#include <x/exception.H>

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <type_traits>

#include <stdint.h>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

//! Replace <, >, and & characters in a string with &lt;, &gt;, and &amp;.

//! The string is defined by a beginning and an ending input iterator.
//! The replacement string gets written to the output iterator.
//!
//! If the input iterator iterates over integer values that are not chars
//! (or unsigned chars), values 0x80 and higher also get replaced with &#x;

template<typename InputIterator, typename OutputIterator>
OutputIterator escape(
	//! Beginning input iterator.
	InputIterator beg_iter,

	//! Ending input iterator.
	InputIterator end_iter,

	//! Output iterator.
	OutputIterator out_iter,

	//! Set this flag to true to also escape quotes, apostrophes, and control characters
	bool escapeqac=false
)
{
	while (beg_iter != end_iter)
	{
		auto ch=*beg_iter++;

		if ( (sizeof(ch) > 1 && (ch & 0x7F) != ch) ||
		     (escapeqac && (ch == '\'' || ch == '"' ||
				    (ch >= 0 && ch < ' '))))
		{
			std::ostringstream o;

			o << "&#x" << std::hex;

			if constexpr (sizeof(ch) == 1)
				o << (int)(unsigned char)ch;
			else if constexpr
				(std::is_same_v<decltype(ch), char32_t>
				 )
				o << (unsigned long long)ch;
			else
				o << ch;

			o << ";";

			std::string s=o.str();

			for (const char *p=s.c_str(); *p; p++)
				*out_iter++=*p;
			continue;
		}

		const char *str;

		switch (ch) {
		case '&':
			str="&amp;";
			break;
		case '<':
			str="&lt;";
			break;
		case '>':
			str="&gt;";
			break;
		default:
			*out_iter++=ch;
			continue;
		}

		while (*str)
			*out_iter++=*str++;
	}
	return out_iter;
}

//! Convenience function for escaping a string

//! Invoke escape() for an ordinary std::string, and return a replacement
//! string value.

std::string escapestr(//! The original string
		      const std::string_view &str,

		      //! Set this flag to true to also escape quotes, apostrophes, and control characters
		      bool escapeqac=false);

//! Convenience function for escaping a string

//! Invoke escape() for an ordinary std::u32string, and return a replacement
//! u32string value.

std::u32string escapestr(//! The original string
			 const std::u32string_view &str,

			 //! Set this flag to true to also escape quotes, apostrophes, and control characters
			 bool escapeqac=false);

//! Convenience function for escaping a string

//! \overload
//! The string to escape gets defined by a beginning and an ending iterator.
template<typename iter_type>
auto escapestr(
	//! Beginning iterator
	iter_type beg_iter,

	//! Ending iterator
	iter_type end_iter,

	//! Whether to replace quotes, apostrophes, and commas.
	bool escapeqac=false)
	-> std::basic_string<std::remove_cvref_t<decltype(*beg_iter)>>
{
	std::basic_string<std::remove_cvref_t<decltype(*beg_iter)>> s;

	if constexpr (
		std::is_same_v<typename std::iterator_traits<iter_type
		>::iterator_category,
		std::random_access_iterator_tag>)
	{
		s.reserve( ((end_iter-beg_iter)/20+1)*21 );
	}
	escape(beg_iter, end_iter, std::back_insert_iterator(s), escapeqac);
	return s;
}

#ifndef DOXYGEN
extern template std::back_insert_iterator<std::string>
escape(std::string::const_iterator, std::string::const_iterator,
       std::back_insert_iterator<std::string>, bool);

extern template std::ostreambuf_iterator<char>
escape(std::string::const_iterator, std::string::const_iterator,
       std::ostreambuf_iterator<char>, bool);

extern template std::back_insert_iterator<std::string>
escape(std::string_view::const_iterator, std::string_view::const_iterator,
       std::back_insert_iterator<std::string>, bool);

extern template std::ostreambuf_iterator<char>
escape(std::string_view::const_iterator, std::string_view::const_iterator,
       std::ostreambuf_iterator<char>, bool);

extern template std::back_insert_iterator<std::u32string>
escape(std::vector<uint32_t>::const_iterator,
       std::vector<uint32_t>::const_iterator,
       std::back_insert_iterator<std::u32string>, bool);

extern template std::string
escapestr(std::string::const_iterator, std::string::const_iterator, bool);

extern template std::string
escapestr(std::string_view::const_iterator,
	  std::string_view::const_iterator, bool);

extern template std::u32string
escapestr(std::vector<char32_t>::const_iterator,
	  std::vector<char32_t>::const_iterator, bool);
#endif

//! Convert a literal string to an XPath expression for a string

//! The beginning and an ending iterator define a sequence for a literal
//! character string, in UTF-8. XPath expression don't have escapes, so
//! xpathescape() uses the third parameter, the output iterator, to
//! produce an Xpath expression:
//!
//! \code
//! concat('string')
//! \endcode
//!
//! If the passed in sequence contains an apostrophe the resulting
//! expression uses a separate parameter to concat() to append an
//! apostrophe, then the rest of the string.

template<typename InputIterator, typename OutputIterator,
	 typename=std::enable_if_t<sizeof(*std::declval<InputIterator>())==1>>
OutputIterator xpathescape(
	//! Beginning input iterator.
	InputIterator beg_iter,

	//! Ending input iterator.
	InputIterator end_iter,

	//! Output iterator.
	OutputIterator out_iter
)
{
	for (const char *concat="concat('"; *concat; ++concat)
		*out_iter++=*concat;

	const char *tail="','')";

	while (beg_iter != end_iter)
	{
		char ch=*beg_iter++;

		if (ch != '\'')
		{
			*out_iter++ = ch;
			continue;
		}

		for (const char *concat="',\"'\",'"; *concat; ++concat)
			*out_iter++=*concat;
		tail="')";
	}

	for ( ; *tail; ++tail)
		*out_iter++ = *tail;

	return out_iter;
}

//! Convenience function for escaping a literal string for an XPath context.

//! Invoke escape() for an ordinary std::string, and return a replacement
//! string value.

std::string xpathescapestr(//! The original string
	const std::string_view &str
);


//! Convenience function for escaping a string

//! \overload
//! The string to escape gets defined by a beginning and an ending iterator.
template<typename iter_type>
std::string xpathescapestr(
	//! Beginning iterator
	iter_type beg_iter,

	//! Ending iterator
	iter_type end_iter)
{
	std::string s;

	if constexpr (
		std::is_same_v<typename std::iterator_traits<iter_type
		>::iterator_category,
		std::random_access_iterator_tag>)
	{
		s.reserve( ((end_iter-beg_iter)/20+1)*21+16);
	}
	xpathescape(beg_iter, end_iter, std::back_insert_iterator{s});

	return s;
}

#ifndef DOXYGEN
extern template std::back_insert_iterator<std::string>
xpathescape(std::string::const_iterator, std::string::const_iterator,
	    std::back_insert_iterator<std::string>);

extern template std::ostreambuf_iterator<char>
xpathescape(std::string::const_iterator, std::string::const_iterator,
	    std::ostreambuf_iterator<char>);

extern template std::string
xpathescapestr(std::string::const_iterator, std::string::const_iterator);

extern template std::string
xpathescapestr(std::string_view::const_iterator,
	       std::string_view::const_iterator);

#endif

#if 0
{
#endif
}

#endif
