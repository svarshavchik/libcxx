/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_param_getenumerationarr_H
#define x_tls_param_getenumerationarr_H

#include <vector>
#include <string>
#include <type_traits>
#include <x/locale.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
	namespace param {
#if 0
	}
}
#endif

//! Implement enumerate() for an enumeration whose values are given in an array

//! The template parameter must define an array called \c manual_enumeration
//! that defines all possible enumerated values, and get_name(), that returns
//! an enumerated value's label.

template <typename base_type>
class getenumerationarr : public base_type {

public:
	//! A specialization defines this to omit some values from enumeration.

	//! If donotenumerate() returns \c true, the enumerated values's
	//! label is not included in the list created by enumerate()

	static bool donotenumerate(typename std::remove_reference<
				   decltype(base_type
					    ::manual_enumeration[0])>
				   ::type) LIBCXX_HIDDEN;

	//! Enumerate names of all enumerated values, into a string vector.

	static void enumerate(//! This vector gets initialized with the string values
			      std::vector<std::string> &labels,

			      //! Locale for the enumerated labels
			      const const_locale &localeRef=
			      locale::base::global());
};

#if 0
{
	{
#endif
	}
}

#endif
