/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_string_argument_H
#define x_string_argument_H

#include <x/namespace.h>
#include <x/tostring.H>
#include <string>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Helper method for accepting a parameter which is a string

//! Standardizes string parameters as std::basic_strings.
//!
//! If a string parameter is specified as a string literal, it gets converted
//! to a std::basic_string.

template<typename char_type>
inline std::basic_string<char_type> string_argument(const char_type * const &s)
{
	return std::basic_string<char_type>(s);
}

//! Helper method for accepting a parameter which is a string

//! Standardizes string parameters as std::basic_strings.
//!
//! If a string parameter is specified as a null-terminated string array,
//! it gets converted to a std::basic_string.

template<typename char_type, size_t n>
inline std::basic_string<char_type> string_argument(const char_type (&s)[n])
{
	return string_argument(&s[0]);
}

//! Helper method for accepting a parameter which is a string

//! Standardizes string parameters as std::basic_strings.
//!
//! Used by string_argument() to convert a container into a
//! std::basic_string.

template<typename basic_string_type> class string_argument_helper {

public:

	//! Construct a std::basic_string from a container, using its begin() and end().

	static inline auto string_argument(const basic_string_type &s)
	{
		return std::basic_string<typename basic_string_type::value_type>
			(std::begin(s), std::end(s));
	}
};

//! Helper method for accepting a parameter which is a string

//! Standardizes string parameters as std::basic_strings.
//!
//! Specialization used by string_argument() when its parameter is already
//! a std::basic_string.

template<typename char_type>
class string_argument_helper<std::basic_string<char_type>> {

 public:

	//! no-op specialization.

	static inline const std::basic_string<char_type>
		&string_argument(const std::basic_string<char_type> &s)
	{
		return s;
	}
};

//! Helper method for accepting a parameter which is a string

//! Standardizes string parameters as std::basic_strings.

template<typename class_type>
inline auto string_argument(const class_type &s)
{
	return string_argument_helper<class_type>
		::string_argument(s);
}

//! Helper method for accepting a parameter which is a std::string

//! Standardizes string parameters as std::string objects.
//! If some other string type is passed (like std::wstring

template<typename class_type>
inline auto basic_string_argument(const class_type &s)
{
	return tostring(string_argument(s));
}

#ifndef DOXYGEN
extern template std::string string_argument(const char * const &);
extern template std::wstring string_argument(const wchar_t * const &);
#endif

#if 0
{
#endif
}
#endif
