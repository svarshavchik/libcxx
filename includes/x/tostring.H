/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tostring_H
#define x_tostring_H

#include <x/namespace.h>
#include <x/locale.H>
#include <x/value_string.H>
#include <x/exception.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Convert some object into a string

//! The default template implementation invokes the class's to_string() method
//! saving the result into a std::string.
template<typename class_type>
inline std::string tostring(const class_type &cl,
			    const const_locale &localeRef)
{
	return value_string<class_type>::to_string(cl, localeRef);
}

//! Convert some object into a string using the current global locale

template<typename class_type>
inline std::string tostring(const class_type &cl)
{
	return tostring(cl,locale::base::global());
}

//! Specialization for a native character string.

template<>
inline std::string tostring(const char * const &cl,
			    const const_locale &localeRef)
{
	return cl;
}

//! Specialization for a native character string.

template<size_t n>
inline std::string tostring(const char (&cl)[n],
			    const const_locale &localeRef)
{
	return cl;
}

//! Specialization for a native character string.

template<>
inline std::string tostring(char * const &cl,
			    const const_locale &localeRef)
{
	return cl;
}

//! Implement conversion functions between a string type and some class type

template<typename class_type> class stringize_impl {

public:
	//! Convert an object to a narrow character string

	static inline std::string tostr(const class_type &cl,
					const const_locale &localeRef=locale::base::global())

	{
		return tostring(cl, localeRef);
	}

	//! Convert an object from a narrow character string

	static inline class_type fromstr(const std::string &str,
					 const const_locale &localeRef=locale::base::global())

	{
		return value_string<class_type>::from_string(str, localeRef);
	}
};

//! Implement conversion functions between a string type, and some class type, with some specializations

template <typename class_type>
class stringize : public stringize_impl<class_type> {
};

//! Specialization for a native char type

template <>
class stringize<const char *>
	: public stringize_impl<std::string > {
};

//! Specialization for a native char type

template <>
class stringize<char *>
	: public stringize_impl<std::string> {
};

//! Specialization for a native char type

template<size_t n>
class stringize<const char[n]>
	: public stringize_impl<std::string > {
};

//! Specialization for a native char type

template<size_t n>
class stringize<char[n]>
	: public stringize_impl<std::string > {
};

#if 0
{
#endif
}
#endif
