/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_param_getenumerationarr_int_H
#define x_param_getenumerationarr_int_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
	namespace param {
#if 0
	}
}
#endif

template <typename base_type>
void getenumerationarr<base_type>
::enumerate(std::vector<std::string> &labels,
	    const const_locale &localeRef)
{
	labels.clear();
	labels.reserve(sizeof(base_type::manual_enumeration)/
		       sizeof(base_type::manual_enumeration[0])-1);

	for (auto value:base_type::manual_enumeration)
	{
		if (donotenumerate(value))
			continue;

		labels.push_back(base_type::get_name(value, localeRef));
	}
}

template <typename base_type>
void getenumerationarr<base_type>
::enumerate(std::vector<std::wstring> &labels,
	    const const_locale &localeRef)
{
	std::vector<std::string> buf;

	enumerate(buf, localeRef);

	labels.clear();
	labels.reserve(buf.size());

	for (auto l:buf)
	{
		labels.push_back(towstring(l, localeRef));
	}
}

#if 0
{
	{
#endif
	}
}

#endif
