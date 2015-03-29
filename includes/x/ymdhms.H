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
OutputIterator ymdhms::toString(OutputIterator iter,
				const const_locale &localeRef,
				const typename OutputIterator::char_type
				*formatString)
		const
{
	typedef typename OutputIterator::char_type CharT;

	std::basic_string<CharT> formatBuf(toString(localeRef, formatString));

	return std::copy(formatBuf.begin(),
			 formatBuf.end(), iter);
}

template<typename char_type>
std::basic_string<char_type>
ymdhms::toString(const const_locale &localeRef,
		 const char_type *formatString)
	const
{
	const char_type deffmt[]=
		{'%', 'a', ',', ' ', '%', 'd', ' ', '%', 'b', ' ', '%', 'Y', ' ', '%', 'H', ':', '%', 'M', ':', '%', 'S', ' ', '%', 'z', 0};

	if (!formatString)
		formatString=deffmt;

	return basic_strftime<char_type>(*this, localeRef)(formatString);
}

//! Convert this date and time value to a string
//! \overload
//!

template<typename char_type>
ymdhms::operator std::basic_string<char_type>()
	const
{
	return toString<char_type>(locale::base::global(), 0);
}

#if 0
{
#endif
}
#endif

