/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_param_onlygetname_H
#define x_param_onlygetname_H

#include <string>

#include <x/param/enumeration.H>
#include <x/locale.H>
#include <x/namespace.h>

#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE {
	namespace param{
#if 0
	}
}
#endif

//! Implement get_enum() for an enumerated value that only implements get_name()

//! A subclass of \ref enumeration "enumeration" that implements
//! a get_name(), but not a get_enum().
//!
//! Implement get_enum() by calling get_name() on each enumerated value, until
//! we find something.

template<typename enum_t>
class onlygetname : public enumeration<enum_t> {

public:

	//! A specialization must implement this.

	static std::string
	get_name(enum_t value, const const_locale &localeRef);

	//! Synthesize get_enum()
	static enum_t get_enum(const std::string &name,
			       const const_locale &localeRef)
	{
		auto charset=localeRef->charset();

		std::string lower_n = unicode::tolower(name, charset);

		for (size_t n=0;
		     n < enumeration<enum_t>::manual_enumeration_cnt;
		     ++n)
		{
			enum_t v=enumeration<enum_t>
				::manual_enumeration[n];

			if (lower_n ==
			    unicode::tolower(get_name(v, localeRef), charset))
				return v;
		}

		nosuchname(name, localeRef);
	}

	//! An invalid name was specified

	static void nosuchname(const std::string &name,
			       const const_locale &localeRef)
		__attribute__((noreturn));

};


#if 0
{
	{
#endif
	}
}

#endif
