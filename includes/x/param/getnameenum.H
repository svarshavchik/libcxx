/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_param_getnameenum_H
#define x_tls_param_getnameenum_H

#include <string>

#include <x/param/enumeration.H>
#include <x/locale.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::param {
#if 0
}
#endif

//! Define conversion functions between an enumerated value and a label
template<typename enum_t>
class getnameenum {

public:

	//! Convert some enumerated value to a label

	static std::string
	get_name(//! Enumerated value to convert
		 enum_t value,

		 //! Locale for conversion
		 const const_locale &localeRef);

	//! Convert a label to an enumerated value
	static enum_t get_enum(//! Label to convert
			       const std::string &name,

			       //! Locale for conversion
			       const const_locale &localeRef);
};


#if 0
{
#endif
}

#endif
