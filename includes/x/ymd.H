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
	std::string dt(formatDate(pattern, localeRef));

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

template<typename InputIterator>
ymd::interval::interval(InputIterator beg_iter,
			InputIterator end_iter,
			const const_locale &localeArg)
	: years(0), months(0), weeks(0), days(0)
{
	construct(beg_iter, end_iter, localeArg);
}

template<typename InputIterator>
void ymd::interval::construct(InputIterator beg_iter,
			      InputIterator end_iter,
			      const const_locale &localeArg)
{
	LIBCXX_NAMESPACE::interval<sdaynum_t> parser(interval_descr, 0,
						     libmsg());

	const std::vector<sdaynum_t> &vec=parser.parse(beg_iter, end_iter);

	days=vec[0];
	weeks=vec[1];
	months=vec[2];
	years=vec[3];
}

//! Common template instantiation

extern template void ymd::interval::construct<std::string::iterator>
(std::string::iterator, std::string::iterator, const const_locale &)
;

#ifndef DOXYGEN
extern template ymd ymd::parser::parse(const char *, const char *);
#endif

#if 0
{
#endif
}

#include <x/strftime.H>

#endif
