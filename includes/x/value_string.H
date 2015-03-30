/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_valuestring_H
#define x_valuestring_H

#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <iterator>
#include <x/is_float.H>
#include <x/locale.H>
#include <x/imbue.H>
#include <x/codecvtiter.H>
#include <x/value_stringable.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Value conversion error exception

void value_conversion_error(const std::string &value)
	__attribute__((noreturn));

//! Value conversion error exception

void value_conversion_error(const std::wstring &value)
	__attribute__((noreturn));

//! No default value for custom classes

void no_custom_class_default()
	__attribute__((noreturn));

template<typename valueType, bool is_integer, bool is_float>
class value_stringimpl;

template<typename valueType> class value_string;

//! The class implements toString(), fromString(),
//! toWideString(), fromWideString().

template<stringable_t, typename valueType> class value_tofromstring {

public:

	//! Convert the value to a string representation.
	static std::string toString(const valueType &v,
				    const const_locale &locale)
	{

		std::string s;

		v.toString(std::back_insert_iterator<std::string>(s), locale);
		return s;
	}

	//! Convert the value from a string represantion
	static valueType fromString(const std::string &s,
				    const const_locale &locale)
	{
		return valueType::fromString(s.begin(), s.end(), locale);
	}

	//! Convert the value to a string representation.
	static std::wstring toWideString(const valueType &v,
					 const const_locale &locale)

	{

		std::wstring s;

		v.toWideString(std::back_insert_iterator<std::wstring>(s),
			       locale);
		return s;
	}

	//! Convert the value from a string represantion
	static valueType fromWideString(const std::wstring &s,
					const const_locale &locale)

	{
		return valueType::fromWideString(s.begin(), s.end(), locale);
	}
};

template<typename valueType> class value_tofromstring<class_towstring,
						      valueType>;

template<typename valueType> class value_tofromstring<class_tostring,
						      valueType>;

template<typename valueType> class value_tofromstring<class_toboth,
						      valueType>;

template<typename valueType> class value_tofromstring<class_tostring_utf8,
						      valueType>;

//! Convert a %value to or from a string representation.

//! This template defined a class with four static methods.
//!
//! - toString(): takes two parameters, the template type value and a locale.
//! The value gets converted to a string representation.
//!
//! - fromString(): takes two a parameters, a string value and a locale.
//! The string value gets converted to the template type value.
//!
//! For \c std::string -s, both functions are a no-op. For natural integer
//! and floating point types, the functions get implemented using
//! \c std::istringstream or \c std::ostringstream. A \c bool specialization
//! uses localized strings \c true and \c false.
//!
//! In addition to string representation conversion, this template defines
//! two more static methods:
//!
//! - initValue(): initial value for options of this type
//!
//! - defaultValue(): the default value for options of this type
//!
//! See \ref option "option" documentation for more information.
//!
//! For natural integer float types, initValue() returns 0 and defaultValue()
//! returns 1. The \c bool initValue() is false, the \c bool defaultValue()
//! is true. The \c std::string initValue() is an empty string, the
//! \c std::string defaultValue() is "1". For custom classes
//! initValue() invokes the default constructor, and defaultValue() throws
//! an %exception. If appropriate, initValue() and defaultValue() may be
//! specialized.
//!
//! \par Converting custom classes to and from strings
//!
//! The template parameter may be a custom class, if it meets the following
//! requirements:
//!
//! \code
//! class customObj {
//!
//! public:
//!
//!     std::string foo;
//!
//!     static const stringable_t stringable=class_tostring;
//!
//!     template<typename OutputIterator>
//!     OutputIterator toString(OutputIterator iter,
//!                             const const_locale &localeRef) const
//!     {
//!         return (std::copy(foo.begin(), foo.end(), iter));
//!     }
//!
//!
//!     template<typename InputIterator>
//!     static customObj fromString(InputIterator beg_iter,
//!                                 InputIterator end_iter,
//!                                 const const_locale &localeArg)
//!
//!     {
//!         customObj c;
//!
//!         c.foo=std::string(beg_iter, end_iter);
//!         return c;
//!     }
//! };
//! \endcode
//!
//! - The custom class must define the static constant stringable. See
//! \ref stringable_t "stringable_t" that specifies what kind of stringification
//! methods it implements.
//!
//! The toString() and toWideString methods in this template invokes the class
//! instance's
//! %toString() method, and passes a
//! \c std::back_insert_iterator for the iterator, however
//! any output iterator may be used in the future.
//! Therefore, the class should define its %toString() and/or %toWideString()
//! as a template that accepts any output iterator.
//!
//! - The custom class must have a static %fromString() and/or
//! %toWideString() method that takes a
//! beginning and an ending iterator, and a locale reference,
//! then constructs and returns an object
//! using the human-readable string representation of the custom object's value.
//!
//! The current implementation always passes string iterators,
//! however eny beginning and ending input iterator may be used in the future,
//! so the class fromString() and/or fromWideString()
//! should be defined as a template.
//!
//! - The custom class should implement a default constructor. initialValue()
//! constructs an object using the default constructor. defaultValue() throws
//! an exception.
//!
//! The following classes in this library implement the required semantics:
//!
//! - \ref hms "hms": converts to "HH:MM:SS".
//!
//! - \ref ymd "ymd": converts to "YYYY-MM-DD".
//!
//! - ymd::interval: converts between localized "x years, y months..." strings.
//!
//! - \ref uriimpl "uriimpl": converts to string representation of a generic URI

template<typename valueType> class value_string
	: public value_stringimpl<valueType,
				  std::numeric_limits<valueType>::is_integer,
				  is_float<valueType>::flag > {
};

//! A \c std::string %value

//! This is a no-op specialization for string values.

template<> class value_string<std::string> {

public:

	//! Convert the value to a string representation.

	static std::string toString(const std::string &v,
				    const const_locale &locale)
	{
		return v;
	}

	//! Convert the value from a string represantion

	static std::string fromString(const std::string &s,
				      const const_locale &locale)
	{
		return s;
	}

	//! Convert the value to a string representation.

	static std::wstring toWideString(const std::string &v,
					 const const_locale &locale)
;

	//! Convert the value from a string represantion

	static std::string fromWideString(const std::wstring &s,
					  const const_locale &locale)
;

	//! The initial value, an empty string.

	static std::string initialValue()
	{
		return "";
	}

	//! The default value.

	static std::string defaultValue()
	{
		return "";
	}
};

//! A \c std::wstring %value

//!

template<> class value_string<std::wstring> {

public:

	//! Convert a wide character string to a character string

	static std::string toString(const std::wstring &v,
				    const const_locale &locale)
;

	//! Convert a narrow character string into a wide character string.

	static std::wstring fromString(const std::string &s,
				       const const_locale &locale)
;

	//! Convert the value to a string representation.

	static std::wstring toWideString(const std::wstring &v,
					 const const_locale &locale)

	{
		return v;
	}

	//! Convert the value from a string represantion

	static std::wstring fromWideString(const std::wstring &v,
					   const const_locale &locale)

	{
		return v;
	}

	//! The initial value, an empty wide string.

	static std::wstring initialValue()
	{
		return L"";
	}

	//! The default value.

	static std::wstring defaultValue()
	{
		return L"";
	}
};

//! A \c bool %value

//! This is a specialization for a boolean value. fromString() takes a
//! that begins with 'F', 'N', or '0' initializes the %value to a false %value,
//! and anything else is considered true.
//! toString() produces a \c "true" or a \c "false".

template<> class value_string<bool>  {

public:

	//! Convert the value to a string representation.

	static std::string toString(bool v,
				    const const_locale &locale)
;

	//! Convert the value from a string represantion

	static bool fromString(const std::string &s,
			       const const_locale &locale)
;

	//! Convert the value to a string representation.

	static std::wstring toWideString(bool v,
					const const_locale &locale)
;

	//! Convert the value from a string represantion

	static bool fromWideString(const std::wstring &s,
				   const const_locale &locale)
;

	//! The initial value, an empty string.

	static bool initialValue()
	{
		return false;
	}

	//! The default value.

	static bool defaultValue()
	{
		return true;
	}
};

//! The %value is a custom class.

//! The toString() and fromString() methods invoke the class's toString()
//! and fromString() methods.

template<typename valueType, bool is_integer, bool is_float>
class value_stringimpl {

public:

	//! Convert the value to a string representation.

	static inline std::string toString(const valueType &v,
					   const const_locale &locale)

	{
		return value_tofromstring<valueType::stringable, valueType>
			::toString(v, locale);
	}

	//! Convert the value from a string represantion

	static valueType fromString(const std::string &s,
				    const const_locale &locale)
	{
		return value_tofromstring<valueType::stringable, valueType>
			::fromString(s, locale);
	}

	//! Convert the value to a string representation.

	static inline std::wstring toWideString(const valueType &v,
					       const const_locale &locale)

	{
		return value_tofromstring<valueType::stringable, valueType>
			::toWideString(v, locale);
	}

	//! Convert the value from a string represantion

	static valueType fromWideString(const std::wstring &s,
					const const_locale &locale)

	{
		return value_tofromstring<valueType::stringable, valueType>
			::fromWideString(s, locale);
	}


	//! The initial value, a default constructor

	static valueType initialValue()
	{
		return valueType();
	}

	//! The default value.

	static valueType defaultValue()
	{
		no_custom_class_default();
		return initialValue();
	}
};

//! A natural integer %value

//! This implementation implements fromString() and toString() for a
//! natural integer %value.

template<typename valueType>
class value_stringimpl<valueType, true, false> {
public:

	//! Return the value of this property, as a string

	//! Convert the value to a string representation.

	static std::string toString(valueType v,
				    const const_locale &locale)
	{
		std::ostringstream o;

		imbue<std::ostringstream> im(locale, o);

		o << v;

		return o.str();
	}

	//! Convert the value from a string represantion

	static valueType fromString(const std::string &s,
				    const const_locale &locale)
	{
		valueType n;

		std::istringstream i(s);

		{
			imbue<std::istringstream> im(locale, i);

			i >> n;
		}

		if (i.fail())
			value_conversion_error(s);

		return n;
	}

	//! Return the value of this property, as a string

	//! Convert the value to a string representation.

	static std::wstring toWideString(valueType v,
					 const const_locale &locale)

	{
		std::wostringstream o;

		imbue<std::wostringstream> im(locale, o);

		o << v;

		return o.str();
	}

	//! Convert the value from a string represantion

	static valueType fromWideString(const std::wstring &s,
					const const_locale &locale)

	{
		valueType n;

		std::wistringstream i(s);

		{
			imbue<std::wistringstream> im(locale, i);

			i >> n;
		}

		if (i.fail())
			value_conversion_error(s);

		return n;
	}

	//! The initial value, zero;

	static valueType initialValue()
	{
		return 0;
	}

	//! The default value, one.

	static valueType defaultValue()
	{
		return 1;
	}
};


//! The floating point %value.

//! The toString() and fromString() methods convert the floating value to
//! a string representation.

template<typename valueType>
class value_stringimpl<valueType, false, true> {
public:

	//! Return the value of this property, as a string

	//! Convert the value to a string representation.

	static std::string toString(valueType v,
				    const const_locale &locale)
	{
		std::ostringstream o;

		o << std::setprecision(std::numeric_limits<valueType>::digits10
				       +2); // See serialize.H

		imbue<std::ostringstream> im(locale, o);

		o << v;

		return o.str();
	}

	//! Convert the value from a string represantion

	static valueType fromString(const std::string &s,
				    const const_locale &locale)
	{
		std::istringstream i(s);

		valueType v;

		{
			imbue<std::istringstream> im(locale, i);

			i >> v;
		}

		if (i.fail())
			value_conversion_error(s);

		return v;
	}

	//! Return the value of this property, as a string

	//! Convert the value to a string representation.

	static std::wstring toWideString(valueType v,
					 const const_locale &locale)

	{
		std::wostringstream o;

		o << std::setprecision(std::numeric_limits<valueType>::digits10
				       +2); // See serialize.H

		{
			imbue<std::wostringstream> im(locale, o);

			o << v;
		}

		return o.str();
	}

	//! Convert the value from a string represantion

	static valueType fromWideString(const std::wstring &s,
					const const_locale &locale)

	{
		std::wistringstream i(s);

		valueType v;

		{
			imbue<std::wistringstream> im(locale, i);

			i >> v;
		}

		if (i.fail())
			value_conversion_error(s);

		return v;
	}

	//! The initial value, zero;

	static valueType initialValue()
	{
		return 0;
	}

	//! The default value, one.

	static valueType defaultValue()
	{
		return 1;
	}
};

//! The class does not implement toString() and fromString(), but implements toWideString() and fromWideString()

template<typename valueType> class value_tofromstring<class_towstring,
						      valueType> {
public:

	//! Convert the value to a string representation.

	static std::string toString(const valueType &v,
				    const const_locale &locale)
	{
		std::string s;

		typedef std::back_insert_iterator<std::string> ins_iter;

		typedef ocodecvtiter<ins_iter>::wtoc_iter_t wtoc_ins_iter;

		v.toWideString(wtoc_ins_iter::create(ins_iter(s), locale),
			       locale).flush();

		return s;
	}

	//! Convert the value from a string represantion

	static valueType fromString(const std::string &s,
				    const const_locale &locale)
	{
		std::wstring w=value_string<std::wstring>
			::fromString(s, locale);

		return valueType::fromWideString(w.begin(), w.end(), locale);
	}

	//! Convert the value to a string representation.

	static std::wstring toWideString(const valueType &v,
					const const_locale &locale)

	{
		std::wstring s;

		v.toWideString(std::back_insert_iterator<std::wstring>(s),
			       locale);

		return s;
	}

	//! Convert the value from a string represantion

	static valueType fromWideString(const std::wstring &w,
					const const_locale &locale)

	{
		return valueType::fromWideString(w.begin(), w.end(), locale);
	}
};

//! This class does not implement toWideString() or fromWideString(), but implements toString() and fromString()

template<typename valueType> class value_tofromstring<class_tostring,
						      valueType> {
public:

	//! Convert the value to a string representation.

	static std::string toString(const valueType &v,
				    const const_locale &locale)

	{
		std::string s;

		v.toString(std::back_insert_iterator<std::string>(s), locale);

		return s;
	}

	//! Convert the value from a string represantion

	static valueType fromString(const std::string &w,
				    const const_locale &locale)

	{
		return valueType::fromString(w.begin(), w.end(), locale);
	}

	//! Convert the value to a string representation.

	static std::wstring toWideString(const valueType &v,
					 const const_locale &locale)

	{
		std::wstring s;

		typedef std::back_insert_iterator<std::wstring> ins_iter;

		typedef ocodecvtiter<ins_iter>::ctow_iter_t ctow_ins_iter;

		v.toString(ctow_ins_iter::create(ins_iter(s), locale),
			   locale).flush();

		return s;
	}

	//! Convert the value from a string represantion

	static valueType fromWideString(const std::wstring &s,
					const const_locale &locale)

	{

		std::string w=value_string<std::wstring>
			::toString(s, locale);

		return valueType::fromString(w.begin(), w.end(), locale);
	}
};


//! The class implements both toString() and toWideString()

template<typename valueType> class value_tofromstring<class_toboth,
						      valueType> {
public:

	//! Convert the value to a string representation.

	static std::string toString(const valueType &v,
				    const const_locale &locale)

	{
		std::string s;

		v.toString(std::back_insert_iterator<std::string>(s), locale);

		return s;
	}

	//! Convert the value from a string represantion

	static valueType fromString(const std::string &w,
				    const const_locale &locale)

	{
		return valueType::fromString(w.begin(), w.end(), locale);
	}

	//! Convert the value to a string representation.

	static std::wstring toWideString(const valueType &v,
					const const_locale &locale)

	{
		std::wstring s;

		v.toWideString(std::back_insert_iterator<std::wstring>(s),
			       locale);

		return s;
	}

	//! Convert the value from a string represantion

	static valueType fromWideString(const std::wstring &w,
					const const_locale &locale)

	{
		return valueType::fromWideString(w.begin(), w.end(), locale);
	}
};

//! This class does not implement toWideString() or fromWideString(), but implements toString() and fromString()

//! The class ignores the locale, and always does it in utf-8.

template<typename valueType> class value_tofromstring<class_tostring_utf8,
						      valueType> {
public:
	//! Convert the value to a string representation.

	static inline std::string toString(const valueType &v,
					   const const_locale &locale)

	{
		return value_string<std::wstring>
			::toString(toWideString(v, locale), locale);
	}

	//! Convert the value from a string represantion

	static inline valueType fromString(const std::string &w,
					   const const_locale &locale)

	{
		return fromWideString(value_string<std::wstring>
				      ::fromString(w, locale), locale);
	}

	//! Convert the value to a string representation.

	static std::wstring toWideString(const valueType &v,
					 const const_locale &localeRef)

	{
		std::wstring s;

		typedef std::back_insert_iterator<std::wstring> ins_iter;

		typedef ocodecvtiter<ins_iter>::ctow_iter_t ctow_ins_iter;

		locale utf8=locale::base::utf8();

		v.toString(ctow_ins_iter::create(ins_iter(s), utf8),
			   utf8).flush();

		return s;
	}

	//! Convert the value from a string represantion

	static valueType fromWideString(const std::wstring &s,
					const const_locale &localeRef)

	{
		locale utf8=locale::base::utf8();

		std::string w=value_string<std::wstring>::toString(s, utf8);

		return valueType::fromString(w.begin(), w.end(), utf8);
	}
};

#if 0
{
#endif
}
#endif
