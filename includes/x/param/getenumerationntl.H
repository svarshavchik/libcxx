/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_param_getenumerationntl_H
#define x_param_getenumerationntl_H

#include <vector>
#include <string>
#include <x/locale.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
	namespace param {
#if 0
	}
}
#endif

//! Implement enumerate() for an enumeration that has a get_ntl()

//! The template parameter specifies a subclass that implements get_ntl(),
//! which returns a null-terminated list of enumerated values, as well as
//! get_name() which converts an enumerated value to a string.
//!
//! Implement enumerate() that initializes a container with labels for the
//! enumerated values.

template <typename base_type>
class getenumerationntl : public base_type {

	//! Pick up the type of the enumerated value from get_enum() in the base class

	typedef decltype(base_type::get_enum("", locale::base::global()))
	enum_t;

public:

	//! Enumerate names of all enumerated values, into a string vector.

	static void enumerate(//! This vector gets initialized with the string values
			      std::vector<std::string> &labels,

			      //! Locale for the enumerated labels
			      const const_locale &localeRef=
			      locale::base::global())
	{
		auto ntl = base_type::get_ntl();

		size_t n;

		for (n=0; ntl[n]; ++n)
			;

		labels.clear();
		labels.reserve(n);

		for (size_t i=0; ntl[i]; ++i)
		{
			if (base_type::donotenumerate(ntl[i]))
			    continue;

			labels.push_back(base_type::get_name((enum_t)
							     ntl[i],
							     localeRef));
		}
	}
};

#if 0
{
	{
#endif
	}
}

#endif
