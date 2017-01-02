/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_param_settable_H
#define x_tls_param_settable_H

#include <x/namespace.h>
#include <x/locale.H>
#include <x/tostring.H>
#include <algorithm>

namespace LIBCXX_NAMESPACE {

	//! Helper templates for stringizing enumerated parameters.

	//! This namespace contains some internal template classes that are
	//! used to implement stringizing of enumerated values, so that they
	//! can be used with fromstring and tostring.

	namespace param {
#if 0
	}
}
#endif

//! Define a container for an enumerated value

//! The container provides methods for converting the enumerated value to and
//! from a text string label, for the enumerated value.
//!
//! The first template parameter is the enumerated type. The second parameter
//! is a superclass that implements get_name(), that converts a value to a
//! a label, and get_enum(), that does the opposite conversion.
//!
//! This is typically \ref getenumerationntl "getenumerationntl" or
//! \ref getenumerationarr "getenumerationarr".
//!
//! The third template parameter is the default enumerated value, for the
//! default constructor.
//!
//! The fourth parameter is either \c class_tostring or \c class_tostring_utf8
//! indicating whether the implementation pays heed to the given locale,
//! or uses latin labels.
//!
//! This class is constructable from either an enumerated value or a string.
//! The "value" member stores the enumerated value. This class can be
//! casted to a string, obtaining the enumerated value's label. It also
//! implements fromString() and toString(), the stringability API.

template<typename enum_type,
	 typename base_type,
	 enum_type defaultValue>
class settable : public base_type {

	//! The initialized value
	enum_type value;

public:
	//! Standard constructor
	settable(enum_type valueArg=defaultValue) noexcept
	: value(valueArg) {}

	//! Destructor
	~settable() {}

	//! Return the raw value

	operator enum_type() const noexcept
	{
		return value;
	}

	//! Construct from a string
	settable(const std::string &value,
		       const const_locale &localeArg=
		       locale::base::global())
		: value(this->get_enum(value, localeArg))
	{
	}

	//! Construct from a string
	settable(const char *value,
		       const const_locale &localeArg=
		       locale::base::global())
		: value(this->get_enum(value, localeArg))
	{
	}

	//! Convert this object to a string.

	template<typename OutputIterator>
	OutputIterator toString(//! Output iterator
				OutputIterator iter,

				//! Locale
				const const_locale &localeRef=
				locale::base::global()) const
	{
		std::string s=this->get_name(value, localeRef);

		return std::copy(s.begin(), s.end(), iter);
	}

	//! Create a sec_param object from a string.

	template<typename InputIterator>
	static settable<enum_type, base_type, defaultValue>
	fromString(//! Beginning iterator
		   InputIterator beg_iter,

		   //! Ending iterator
		   InputIterator end_iter,

		   //! Locale
		   const const_locale &localeArg=locale::base::global())
	{
		return settable<enum_type, base_type, defaultValue>
			(std::string(beg_iter, end_iter));
	}
};

#if 0
{
	{
#endif
	}
}

#endif
