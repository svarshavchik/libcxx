/*
** Copyright 2012 Double Precision, Inc.
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

//! The default template implementation invokes the class's toString() method
//! saving the result into a std::string.
template<typename class_type>
inline std::string tostring(const class_type &cl,
			    const const_locale &localeRef)
{
	return value_string<class_type>::toString(cl, localeRef);
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

//! Specialization for a native wide character string

template<>
inline std::string tostring(const wchar_t * const &cl,
			    const const_locale &localeRef)
{
	return tostring(std::wstring(cl), localeRef);
}

//! Specialization for a native character string.

template<>
inline std::string tostring(char * const &cl,
			    const const_locale &localeRef)
{
	return cl;
}

//! Specialization for a native wide character string

template<>
inline std::string tostring(wchar_t * const &cl,
			    const const_locale &localeRef)
{
	return tostring(std::wstring(cl), localeRef);
}

//! Specialization for a native wide character string

template<size_t n>
inline std::string tostring(const wchar_t (&cl)[n],
			    const const_locale &localeRef)
{
	return tostring(&cl[0], localeRef);
}

//! Convert some object into a wide string

//! The default template implementation invokes the class's toWideString()
//! method saving the result into a std::string.
template<typename class_type>
inline std::wstring towstring(const class_type &cl,
			      const const_locale &localeRef)
{
	return value_string<class_type>::toWideString(cl, localeRef);
}

//! Convert some object into a wide string using the current global locale

template<typename class_type>
inline std::wstring towstring(const class_type &cl)
{
	return towstring(cl, locale::base::global());
}

//! Specialization for a native character string.

template<>
inline std::wstring towstring(const char * const &cl,
			      const const_locale &localeRef)
{
	return towstring(std::string(cl), localeRef);
}

//! Specialization for a native character string.

template<size_t n>
inline std::wstring towstring(const char (&cl)[n],
			      const const_locale &localeRef)
{
	return towstring(&cl[0], localeRef);
}

//! Specialization for a native wide character string

template<>
inline std::wstring towstring(const wchar_t * const &cl,
			      const const_locale &localeRef)
{
	return cl;
}

//! Specialization for a native character string.

template<>
inline std::wstring towstring(char * const &cl,
			      const const_locale &localeRef)
{
	return towstring(std::string(cl), localeRef);
}

//! Specialization for a native wide character string

template<>
inline std::wstring towstring(wchar_t * const &cl,
			      const const_locale &localeRef)
{
	return cl;
}

//! Specialization for a native character string.

template<size_t n>
inline std::wstring towstring(const wchar_t (&cl)[n],
			      const const_locale &localeRef)
{
	return cl;
}

//! Convert string in some locale to a UTF-8 string

template<typename string_type>
std::string toutf8string(string_type &&type, const const_locale &localeRef)
{
	return tostring(towstring(std::forward<string_type>(type),
				  localeRef),
			locale::base::utf8());
}

//! Convert string in global locale to a UTF-8 string

template<typename string_type>
std::string toutf8string(string_type &&type)
{
	return toutf8string(std::forward<string_type>(type),
			    locale::base::global());
}

//! Convert UTF-8 string to a string in some locale

template<typename string_type>
std::string fromutf8string(string_type &&type, const const_locale &localeRef)
{
	return tostring(towstring(std::forward<string_type>(type),
				  locale::base::utf8()), localeRef);
}

//! Convert UTF-8 string to string in global locale

template<typename string_type>
std::string fromutf8string(string_type &&type)
{
	return fromutf8string(std::forward<string_type>(type),
			      locale::base::global());
}

//! Implement conversion functions between a string type and some class type

template<typename string_type, typename class_type> class stringize_impl;

//! Specialization for conversions to/from a std::string

template<typename class_type> class stringize_impl<std::string, class_type> {

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
		return value_string<class_type>::fromString(str, localeRef);
	}
};

//! Specialization for convertsions to/from a std::wstring

template<typename class_type> class stringize_impl<std::wstring, class_type> {

public:

	//! Convert an object to a wide character string

	static inline std::wstring tostr(const class_type &cl,
					 const const_locale &localeRef=locale::base::global())

	{
		return towstring(cl, localeRef);
	}

	//! Convert an object from a wide character string

	static inline class_type fromstr(const std::wstring &str,
					 const const_locale &localeRef=locale::base::global())

	{
		return value_string<class_type>::fromWideString(str, localeRef);
	}
};

//! Implement conversion functions between a string type, and some class type, with some specializations

template <typename string_type, typename class_type>
class stringize : public stringize_impl<string_type, class_type> {
};

//! Specialization for a native char type

template <typename string_type, typename char_type>
class stringize<string_type, const char_type *>
	: public stringize_impl<string_type, std::basic_string<char_type> > {
};

//! Specialization for a native char type

template <typename string_type, typename char_type>
class stringize<string_type, char_type *>
	: public stringize_impl<string_type, std::basic_string<char_type> > {
};

//! Specialization for a native char type

template<typename string_type, typename char_type, size_t n>
class stringize<string_type, const char_type[n]>
	: public stringize_impl<string_type, std::basic_string<char_type> > {
};

//! Specialization for a native char type

template<typename string_type, typename char_type, size_t n>
class stringize<string_type, char_type[n]>
	: public stringize_impl<string_type, std::basic_string<char_type> > {
};

#ifndef DOXYGEN
extern template std::string fromutf8string(const std::string &,
					   const const_locale &);
extern template std::string fromutf8string(std::string &&,
					   const const_locale &);
extern template std::string fromutf8string(const std::string &);
extern template std::string fromutf8string(std::string &&);

extern template std::string toutf8string(const std::string &,
					 const const_locale &);
extern template std::string toutf8string(std::string &&,
					 const const_locale &);
extern template std::string toutf8string(const std::string &);
extern template std::string toutf8string(std::string &&);
#endif

#if 0
{
#endif
}
#endif
