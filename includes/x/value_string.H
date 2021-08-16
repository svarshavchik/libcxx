/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_valuestring_H
#define x_valuestring_H

#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <iterator>
#include <type_traits>
#include <x/locale.H>
#include <x/imbue.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Value conversion error exception

void value_conversion_error(const std::string &value)
	__attribute__((noreturn));


//! Convert a %value to or from a string representation.

//! This template defined a class with two static methods.
//!
//! - to_string(): takes two parameters, the template type value and a locale.
//! The value gets converted to its string representation.
//!
//! - from_string(): takes two parameters, a string value and a locale.
//! The value's string representation gets converted to the template type value.
//!
//! For \c std::string -s, both functions are a no-op. For natural integer
//! and floating point types, the functions get implemented using
//! \c std::istringstream or \c std::ostringstream. A \c bool specialization
//! uses localized strings \c true and \c false.
//!
//! In addition to string representation conversion, this template defines
//! two more static methods:
//!
//! - initial_value(): initial value for options of this type
//!
//! - default_value(): the default value for options of this type
//!
//! See \ref option "option" documentation for more information.
//!
//! For natural integer float types, initial_value() returns 0 and
//! default_value()
//! returns 1. The \c bool initial_value() is false, the \c bool default_value()
//! is true. The \c std::string initial_value() is an empty string, the
//! \c std::string default_value() is "1". For custom classes
//! initial_value() invokes the default constructor, and default_value() throws
//! an %exception. If appropriate, initial_value() and default_value() may be
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
//!     template<typename OutputIterator>
//!     OutputIterator to_string(OutputIterator iter,
//!                             const const_locale &localeRef) const
//!     {
//!         return (std::copy(foo.begin(), foo.end(), iter));
//!     }
//!
//!
//!     template<typename InputIterator>
//!     static customObj from_string(InputIterator beg_iter,
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
//! The to_string() method in this template invokes the class
//! instance's
//! %to_string() method, and passes a
//! \c std::back_insert_iterator for the iterator, however
//! any output iterator may be used in the future.
//! Therefore, the class should define its %to_string()
//! as a template that accepts any output iterator.
//!
//! - The custom class must have a static %from_string() method that takes a
//! beginning and an ending iterator, and a locale reference,
//! then constructs and returns an object
//! using the human-readable string representation of the custom object's value.
//!
//! The current implementation always passes string iterators,
//! however eny beginning and ending input iterator may be used in the future,
//! so the class from_string()
//! should be defined as a template.
//!
//! - The custom class should implement a default constructor, and a
//! default_value() static class method. initial_value()
//! constructs an object using the default constructor. default_value()
//! calls the class's default_value() method.

template<typename value_type, typename=void> class value_string;

//! A \c std::string %value

//! This is a no-op specialization for string values.

template<> class value_string<std::string> {

public:

	//! Convert the value to a string representation.

	static std::string to_string(const std::string &v,
				     const const_locale &locale)
	{
		return v;
	}

	//! Convert the value from a string represantion

	static std::string from_string(const std::string &s,
				       const const_locale &locale)
	{
		return s;
	}

	//! The initial value, an empty string.

	static std::string initial_value()
	{
		return "";
	}

	//! The default value.

	static std::string default_value()
	{
		return "";
	}
};

//! A \c bool %value

//! This is a specialization for a boolean value. from_string() takes a
//! that begins with 'F', 'N', or '0' initializes the %value to a false %value,
//! and anything else is considered true.
//! to_string() produces a \c "true" or a \c "false".

template<> class value_string<bool>  {

public:

	//! Convert the value to a string representation.

	static std::string to_string(bool v,
				     const const_locale &locale);

	//! Convert the value from a string represantion

	static bool from_string(const std::string &s,
				const const_locale &locale);


	//! The initial value, an empty string.

	static constexpr bool initial_value()
	{
		return false;
	}

	//! The default value.

	static constexpr bool default_value()
	{
		return true;
	}
};

//! Default \ref value_string "value_string" implementation of default_value().

//! The default implementation does not implement default_value() when the
//! class doesn't have its own default_value() method.
//!
//! This class gets inherited by the default implementation of
//! \ref value_string "value_string".
template<typename T, typename=void> struct value_default_value {

};

//! Specialization for classes that have a default_value() static method.

//! Defines default_value() that gets forwarded to it.

template<typename T>
struct value_default_value<T, std::void_t<decltype(T::default_value())>> {

	//! The default value

	static inline auto default_value()
	{
		return T::default_value();
	}

};

// Default implementation.

template<typename value_type, typename> class value_string
	: public value_default_value<value_type> {


public:

	//! Convert the value to a string representation.

	static inline std::string to_string(const value_type &v,
					    const const_locale &locale)

	{
		std::string s;

		v.to_string(std::back_insert_iterator<std::string>(s), locale);

		return s;
	}

	//! Convert the value from a string represantion

	static auto from_string(const std::string &s,
				const const_locale &locale)
	{
		return value_type::from_string(s.begin(), s.end(), locale);
	}

	//! The initial value, a default constructor

	template<typename T=value_type,
		 typename =std::enable_if_t<std::is_constructible_v<T>>>
	static auto initial_value()
	{
		return value_type{};
	}
};

//! A natural integer %value

//! This implementation implements from_string() and to_string() for a
//! natural integer %value.

template<typename value_type>
class value_string<value_type,
		   std::enable_if_t<std::numeric_limits<value_type>::is_integer>>
{
public:

	//! Return the value of this property, as a string

	//! Convert the value to a string representation.

	static std::string to_string(value_type v,
				     const const_locale &locale)
	{
		std::ostringstream o;

		imbue<std::ostringstream> im(locale, o);

		o << v;

		return o.str();
	}

	//! Convert the value from a string represantion

	static value_type from_string(const std::string &s,
				      const const_locale &locale)
	{
		value_type n;

		std::istringstream i(s);

		{
			imbue<std::istringstream> im(locale, i);

			i >> n;
		}

		if (i.fail())
			value_conversion_error(s);

		return n;
	}

	//! The initial value, zero;

	static value_type initial_value()
	{
		return 0;
	}

	//! The default value, one.

	static value_type default_value()
	{
		return 1;
	}
};


//! The floating point %value.

//! The to_string() and from_string() methods convert the floating value to
//! a string representation.

template<typename value_type>
class value_string<value_type,
		   std::enable_if_t<
			   std::numeric_limits<value_type>::is_specialized &&
			   !std::numeric_limits<value_type>::is_integer>> {

public:

	//! Return the value of this property, as a string

	//! Convert the value to a string representation.

	static std::string to_string(value_type v,
				     const const_locale &locale)
	{
		std::ostringstream o;

		o << std::setprecision(std::numeric_limits<value_type>::digits10
				       +2); // See serialize.H

		imbue<std::ostringstream> im(locale, o);

		o << v;

		return o.str();
	}

	//! Convert the value from a string represantion

	static value_type from_string(const std::string &s,
				      const const_locale &locale)
	{
		std::istringstream i(s);

		value_type v;

		{
			imbue<std::istringstream> im(locale, i);

			i >> v;
		}

		if (i.fail())
			value_conversion_error(s);

		return v;
	}

	//! The initial value, zero;

	static value_type initial_value()
	{
		return 0;
	}

	//! The default value, one.

	static value_type default_value()
	{
		return 1;
	}
};

#if 0
{
#endif
}
#endif
