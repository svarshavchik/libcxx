/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ymdhms_H
#define x_ymdhms_H

#include <x/ymdhmsfwd.H>
#include <x/strftime.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename OutputIterator>
inline OutputIterator ymdhms::formatter::toString(OutputIterator iter,
						  const const_locale &localeRef)
	const
{
	std::string s=basic_strftime<char>(obj, localeRef)(format_string);

	return std::copy(s.begin(), s.end(), iter);
}

template<typename OutputIterator>
OutputIterator ymdhms::formatter::toWideString(OutputIterator iter,
					       const const_locale &localeRef)
	const
{
	std::string s=basic_strftime<char>(obj, localeRef)(format_string);

	return std::copy(s.begin(), s.end(), iter);
}

#if 0
{
#endif
}
#endif
