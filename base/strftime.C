/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/strftime.H"
#include "x/ymdhms.H"
#include "x/imbue.H"
#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

strftime_base::strftime_base(const const_locale &lArg)
	: l(lArg),
	  ios(NULL),
	  facetRef(l->get_facet< strftime_ref_t >()),
	  facetRefImpl(facetRef->getFacetConstRef())
{
	ios.clear();
}

strftime_base::~strftime_base()
{
}

strftime::strftime(const const_tzfile &tzArg,
		   const const_locale &lArg)
	: strftime_base(lArg), tz(tzArg)
{
	operator()(time(0), tzArg);
}

strftime::strftime(const ymdhms &datetime,
		   const const_locale &lArg)
	: strftime_base(lArg),
	  tz(datetime.getTimezone())
{
	operator()(datetime);
}

strftime::strftime(const ymd &dateValue,
		   const const_locale &lArg,
		   const const_tzfile &tzArg)
	: strftime_base(lArg), tz(tzArg)
{
	operator()(dateValue, tzArg);
}

strftime::strftime(const hms &time,
		   const const_locale &lArg)
	: strftime_base(lArg),
	  tz(tzfile::base::local())
{
	operator()(time);
}

strftime::strftime(time_t t,
		   const const_tzfile &tzArg,
		   const const_locale &lArg)
	: strftime_base(lArg), tz(tzArg)
{
	operator()(t, tzArg);
}

strftime::strftime(const struct tm *tmArg,
		   const const_locale &lArg)
	: strftime_base(lArg),
	  tz(tzfile::base::local())
{
	operator()(tmArg);
}

strftime::~strftime()
{
}

strftime &strftime::operator()(const struct tm *tm_cpy) noexcept
{
	tm= *tm_cpy;
	return *this;
}

strftime &strftime::operator()(time_t timeVal)

{
	return operator()(timeVal, tz);
}

strftime &strftime::operator()(time_t timeVal,
			       const const_tzfile &tzArg)

{
	return operator()(ymdhms(timeVal, tzArg));
}

strftime &strftime::operator()(const ymdhms &datetime)

{
	tm=datetime;
	tm_tz=tm.tm_zone;
	tm.tm_zone=const_cast<char *>(tm_tz.c_str());
	// Make sure we own the buffer this points to

	tz=datetime.getTimezone();
	return *this;
}

strftime &strftime::operator()(const ymd &date)

{
	return operator()(date, tz);
}

strftime &strftime::operator()(const ymd &date,
			       const const_tzfile &tzArg)

{
	return operator()(ymdhms(date, hms(0, 0, 0), tzArg));
}

strftime &strftime::operator()(const hms &time)

{
	return operator()(ymdhms(ymdhms::epoch(), time, tzfile::base::utc()));
}

std::string strftime::operator()(const std::string_view &str)
{
	std::ostringstream o;

	operator()(str, o);

	return o.str();
}

std::u32string strftime::operator()(const std::u32string_view &str)
{
	std::string s;
	bool flag=false;

	unicode::iconvert::fromu::convert(str.begin(), str.end(),
					  l->charset(), s, flag);

	return unicode::iconvert::tou::convert(operator()(s),
					       l->charset()).first;
}

strftime &strftime::operator()(const std::string_view &str,
			       std::ostream &o)
{
	imbue<std::ios> im{l, ios};

	strftime_base::
		facetRefImpl.put( std::ostreambuf_iterator<char>(o),
				  ios, ' ', &tm, str.begin(),
				  str.end());
	return *this;
}

#if 0
{
#endif
}
