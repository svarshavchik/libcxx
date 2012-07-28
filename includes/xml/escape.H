/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_escape_H
#define x_xml_escape_H

#include <x/namespace.h>
#include <x/exception.H>
#include <x/iter_value.H>

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iterator>

#include <stdint.h>

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

//! Replace <, >, and & characters in a string with &lt;, &gt;, and &amp;.

//! The string is defined by a beginning and an ending input iterator.
//! The replacement string gets written to the output iterator.
//!
//! If the input iterator iterates over integer values that are not chars
//! (or unsigned chars), values 0x80 and higher also get replaced with &#x;

template<typename InputIterator, typename OutputIterator>
OutputIterator escape(//! Beginning input iterator.
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
		typename iter_value_type<InputIterator>::value_t ch=*beg_iter++;

		if ( (sizeof(ch) > 1 && (ch & 0x7F) != ch) ||
		     (escapeqac && (ch == '\'' || ch == '"' ||
				    (ch >= 0 && ch < ' '))))
		{
			std::ostringstream o;

			o << "&#x" << std::hex;

			if (sizeof(ch) == 1)
				o << (int)(unsigned char)ch;
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
		      const std::string &str,

		      //! Set this flag to true to also escape quotes, apostrophes, and control characters
		      bool escapeqac=false);

template<typename iter_type>
std::string escapestr(//! Beginning iterator
		      iter_type beg_iter,

		      //! Ending iterator
		      iter_type end_iter,

		      bool escapeqac=false)
{
	std::string s;

	escape(beg_iter, end_iter, std::back_insert_iterator<std::string>(s),
	       escapeqac);
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
escape(std::vector<int16_t>::const_iterator,
       std::vector<int16_t>::const_iterator,
       std::back_insert_iterator<std::string>, bool);

extern template std::ostreambuf_iterator<char>
escape(std::vector<int16_t>::const_iterator,
       std::vector<int16_t>::const_iterator,
       std::ostreambuf_iterator<char>, bool);

extern template std::back_insert_iterator<std::string>
escape(std::vector<uint16_t>::const_iterator,
       std::vector<uint16_t>::const_iterator,
       std::back_insert_iterator<std::string>, bool);

extern template std::ostreambuf_iterator<char>
escape(std::vector<uint16_t>::const_iterator,
       std::vector<uint16_t>::const_iterator,
       std::ostreambuf_iterator<char>, bool);

extern template std::back_insert_iterator<std::string>
escape(std::vector<int32_t>::const_iterator,
       std::vector<int32_t>::const_iterator,
       std::back_insert_iterator<std::string>, bool);

extern template std::ostreambuf_iterator<char>
escape(std::vector<int32_t>::const_iterator,
       std::vector<int32_t>::const_iterator,
       std::ostreambuf_iterator<char>, bool);

extern template std::back_insert_iterator<std::string>
escape(std::vector<uint32_t>::const_iterator,
       std::vector<uint32_t>::const_iterator,
       std::back_insert_iterator<std::string>, bool);

extern template std::ostreambuf_iterator<char>
escape(std::vector<uint32_t>::const_iterator,
       std::vector<uint32_t>::const_iterator,
       std::ostreambuf_iterator<char>, bool);

extern template std::string
escapestr(std::string::const_iterator, std::string::const_iterator, bool);

extern template std::string
escapestr(std::vector<int16_t>::const_iterator,
	  std::vector<int16_t>::const_iterator, bool);

extern template std::string
escapestr(std::vector<uint16_t>::const_iterator,
	  std::vector<uint16_t>::const_iterator, bool);

extern template std::string
escapestr(std::vector<int32_t>::const_iterator,
	  std::vector<int32_t>::const_iterator, bool);

extern template std::string
escapestr(std::vector<uint32_t>::const_iterator,
	  std::vector<uint32_t>::const_iterator, bool);


#endif

#if 0
{
	{
#endif
	}
}

#endif
