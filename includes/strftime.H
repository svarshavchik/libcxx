/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_strftime_H
#define x_strftime_H

#include <x/strftimefwd.H>
#include <x/ymdhmsfwd.H>
#include <x/imbue.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename char_type>
basic_strftime_base<char_type>::basic_strftime_base(const const_locale &lArg)
: l(lArg),
			  ios(NULL),
			  facetRef(l->get_facet< basic_strftime_ref_t >()),
			  facetRefImpl(facetRef->getFacetConstRef())
{
	ios.clear();
}

template<typename char_type>
basic_strftime_base<char_type>::~basic_strftime_base() noexcept
{
}

template<typename char_type>
basic_strftime<char_type>::basic_strftime(const const_tzfile &tzArg,
					  const const_locale &lArg)
: basic_strftime_base<char_type>(lArg),
			  tz(tzArg)
{
	operator()(time(0), tzArg);
}

template<typename char_type>
basic_strftime<char_type>::basic_strftime(const ymdhms &datetime,
					  const const_locale &lArg)
: basic_strftime_base<char_type>(lArg),
			  tz(datetime.getTimezone())
{
	operator()(datetime);
}

template<typename char_type>
basic_strftime<char_type>::basic_strftime(const ymd &dateValue,
					  const const_locale &lArg,
					  const const_tzfile &tzArg)
: basic_strftime_base<char_type>(lArg), tz(tzArg)
{
	operator()(dateValue, tzArg);
}

template<typename char_type>
basic_strftime<char_type>::basic_strftime(const hms &time,
					  const const_locale &lArg)
: basic_strftime_base<char_type>(lArg),
			  tz(tzfile::base::local())
{
	operator()(time);
}
		       
template<typename char_type>
basic_strftime<char_type>::basic_strftime(time_t t,
					  const const_tzfile &tzArg,
					  const const_locale &lArg)
: basic_strftime_base<char_type>(lArg), tz(tzArg)
{
	operator()(t, tzArg);
}

template<typename char_type>
basic_strftime<char_type>::basic_strftime(const struct tm *tmArg,
					  const const_locale &lArg)
: basic_strftime_base<char_type>(lArg),
			  tz(tzfile::base::local())
{
	operator()(tmArg);
}

template<typename char_type>
basic_strftime<char_type>::~basic_strftime() noexcept
{
}

template<typename char_type>
basic_strftime<char_type> &
basic_strftime<char_type>::operator()(const struct tm *tm_cpy) noexcept
{
	tm= *tm_cpy;
	return *this;
}

template<typename char_type>
basic_strftime<char_type> &
basic_strftime<char_type>::operator()(time_t timeVal)

{
	return operator()(timeVal, tz);
}

template<typename char_type>
basic_strftime<char_type> &
basic_strftime<char_type>::operator()(time_t timeVal,
				      const const_tzfile &tzArg)

{
	return operator()(ymdhms(timeVal, tzArg));
}

template<typename char_type>
basic_strftime<char_type> &
basic_strftime<char_type>::operator()(const ymdhms &datetime)

{
	tm=datetime;
	tm_tz=tm.tm_zone;
	tm.tm_zone=
		const_cast<char *>(tm_tz.c_str());
	// Make sure we own the buffer this points to

	tz=datetime.getTimezone();
	return *this;
}

template<typename char_type>
basic_strftime<char_type> &
basic_strftime<char_type>::operator()(const ymd &date)

{
	return operator()(date, tz);
}

template<typename char_type>
basic_strftime<char_type> &
basic_strftime<char_type>::operator()(const ymd &date,
				      const const_tzfile &tzArg)

{
	return operator()(ymdhms(date, hms(0, 0, 0), tzArg));
}

template<typename char_type>
basic_strftime<char_type> &
basic_strftime<char_type>::operator()(const hms &time)

{
	return operator()(ymdhms(ymdhms::epoch(), time, tzfile::base::utc()));
}

template<typename char_type>
std::basic_string<char_type>
basic_strftime<char_type>::operator()(const std::basic_string<char_type> &str)

{
	return operator()(str.c_str());
}

template<typename char_type>
std::basic_string<char_type>
basic_strftime<char_type>::operator()(const char_type *str)

{
	std::basic_ostringstream<char_type> o;

	this->operator()(str, o);

	return o.str();
}

template<typename char_type>
basic_strftime<char_type> &
basic_strftime<char_type>::operator()(const std::basic_string<char_type> &str,
		std::basic_ostream<char_type> &o)

{
	return this->operator()(str.c_str(), o);
}

template<typename char_type>
basic_strftime<char_type> &
basic_strftime<char_type>::operator()(const char_type *str,
				      std::basic_ostream<char_type> &o)

{
	const char_type *strend;

	for (strend=str; *strend; ++strend)
		;

	imbue<std::basic_ios<char_type> > im(this->l, this->ios);

	basic_strftime_base<char_type>::
		facetRefImpl.put( std::ostreambuf_iterator<char_type>(o),
				  this->ios, ' ', &this->tm, str, strend);
	return *this;
}

#if 0
{
#endif
}
#endif
