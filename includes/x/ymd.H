/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ymd_H
#define x_ymd_H

#include <x/ymdfwd.H>
#include <x/strftime.H>
#include <x/messagesfwd.H>
#include <x/interval.H>
#include <x/locale.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename OutputIterator>
OutputIterator ymd::toString(OutputIterator iter,
			     const const_locale &localeRef,
			     const char *pattern)
	const
{
	std::string dt(format_date(pattern, localeRef));

	return std::copy(dt.begin(), dt.end(), iter);
}

template<typename OutputIterator>
OutputIterator ymd::toString(OutputIterator iter,
			     const const_locale &localeRef,
			     const std::string &pattern)
	const
{
	return toString(iter, pattern.c_str(), localeRef);
}

template<typename OutputIterator>
OutputIterator ymd::interval::toString(OutputIterator iter,
				       const const_locale &localeRef) const
{
	std::string str(this->internal_tostring(localeRef));

	return std::copy(str.begin(), str.end(), iter);
}

#if 0
{
#endif
}

#include <x/strftime.H>

#endif
