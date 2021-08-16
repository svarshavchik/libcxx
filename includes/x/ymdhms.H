/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ymdhms_H
#define x_ymdhms_H

#include <x/ymdhmsfwd.H>
#include <x/strftime.H>
#include <x/namespace.h>
#include <ostream>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename OutputIterator>
inline OutputIterator ymdhms::formatter::to_string(OutputIterator iter,
						  const const_locale &localeRef)
	const
{
	std::string s=strftime(obj, localeRef)(format_string);

	return std::copy(s.begin(), s.end(), iter);
}

//! Overload the << operator.

std::ostream &operator<<(std::ostream &o, const ymdhms &);

#if 0
{
#endif
}
#endif
