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

//! Standardizes string parameters as std::strings.

template<typename class_type>
inline std::string string_argument(const class_type &s)
{
	return std::string(s.begin(), s.end());
}

//! Helper method for accepting a parameter which is a string

//! Standardizes string parameters as std::strings.
//!
//! If a string parameter is specified as a string literal, it gets converted
//! to a std::string.

template<>
inline std::string string_argument(const std::string &s)
{
	return s;
}

//! Helper method for accepting a parameter which is a string

//! Standardizes string parameters as std::strings.
//!
//! If a string parameter is specified as a string literal, it gets converted
//! to a std::string.

template<>
inline std::string string_argument(const char * const &s)
{
	return std::string(s);
}

//! Helper method for accepting a parameter which is a string

//! Standardizes string parameters as std::strings.
//!
//! If a string parameter is specified as a null-terminated string array,
//! it gets converted to a std::basic_string.

template<size_t n>
inline std::string string_argument(const char (&s)[n])
{
	return string_argument(&s[0]);
}

//! Helper method for accepting a parameter which is a std::string

//! Standardizes string parameters as std::string objects.

template<typename class_type>
inline auto basic_string_argument(const class_type &s)
{
	return string_argument(s);
}

#ifndef DOXYGEN
extern template std::string string_argument(const char * const &);
#endif

#if 0
{
#endif
}
#endif
