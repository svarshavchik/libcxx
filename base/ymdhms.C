/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ymdhms.H"
#include <x/exception.H>
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

ymdhms::operator struct tm() const
{
	struct tm tmValue=tm();

	tmValue.tm_sec=s;
	tmValue.tm_min=m;
	tmValue.tm_hour=h;
	tmValue.tm_mday=getDay();
	tmValue.tm_mon=getMonth()-1;
	tmValue.tm_year=getYear()-1900;
	tmValue.tm_wday=getDayOfWeek();
	tmValue.tm_yday= static_cast<const ymd &>(*this)
		- ymd(getYear(), 1, 1);
	tmValue.tm_isdst=getAltzone() ? 1:0;
	tmValue.tm_gmtoff=getUTCoffset();
	tmValue.tm_zone=const_cast<char *>(getTzname().c_str());

	return tmValue;
}

ymdhms::ymdhms(time_t timevalue,
	       const const_tzfile &tzfileArg)
		: ymdhmsBase( tzfileArg->compute_ymdhms(timevalue)),
		  tz(tzfileArg)
{
}

ymdhms::ymdhms(const ymdhms &timevalue,
	       const const_tzfile &tzfileArg)
	: ymdhmsBase(tzfileArg->compute_ymdhms((time_t)timevalue)),
	  tz(tzfileArg)
{
}

ymdhms::ymdhms(const ymd &ymdVal,
	       const hms &hmsVal,
	       const const_tzfile &tzfileArg)
	: ymdhmsBase(tzfileArg->compute_ymdhms(ymdVal, hmsVal)),
	  tz(tzfileArg)
{
}

ymdhms::ymdhms(const struct tm &timeValue,
	       const const_tzfile &tzfileArg)
	: ymdhmsBase(tzfileArg->compute_ymdhms(timeValue)),
	  tz(tzfileArg)
{
}

ymdhms::~ymdhms() noexcept
{
}

bool ymdhms::operator<(const ymdhms &o) const
{
	if (getUTCoffset() != o.getUTCoffset())
		return tz->compute_time_t(*this) <
			tz->compute_time_t(o);

	if (ymd::operator<(o))
		return true;
	if (ymd::operator>(o))
		return false;
	return hms::operator<(o);
}

bool ymdhms::operator>(const ymdhms &o) const
{
	if (getUTCoffset() != o.getUTCoffset())
		return tz->compute_time_t(*this) <
			tz->compute_time_t(o);

	if (ymd::operator>(o))
		return true;
	if (ymd::operator<(o))
		return false;
	return hms::operator>(o);
}

bool ymdhms::operator==(const ymdhms &o) const
{
	if (getUTCoffset() != o.getUTCoffset())
		return tz->compute_time_t(*this) <
			tz->compute_time_t(o);

	return ymd::operator==(o) && hms::operator==(o);
}

bool ymdhms::operator!=(const ymdhms &o) const
{
	return !operator==(o);
}

bool ymdhms::operator<=(const ymdhms &o) const
{
	return !operator>(o);
}

bool ymdhms::operator>=(const ymdhms &o) const
{
	return !operator<(o);
}

void ymdhms::cannot_parse_time()
{
	throw EXCEPTION(libmsg(_txt("Cannot parse time value")));
}


ymdhms::formatter::formatter(const ymdhms &objArg)
	: obj(objArg),
	  format_string("%a, %d %b %Y %H:%M:%S %z")
{
}

ymdhms::operator std::string() const
{
	return tostring(formatter(*this));
}

ymdhms::operator std::wstring() const
{
	return towstring(formatter(*this));
}

static std::string pick_short_format(const ymdhms &objArg,
				     const ymdhms &now)
{
	ymdhms six_months_before=now;

	static_cast<ymd &>(six_months_before) -= ymd::interval(180);

	return objArg <= now && objArg >= six_months_before
		? "%b %d %H:%M":"%b %d  %Y";
}

ymdhms::short_formatter::short_formatter(const ymdhms &objArg)
	: short_formatter(objArg,
			  ymdhms(time(0), objArg.getTimezone()))
{
}

ymdhms::short_formatter::short_formatter(const ymdhms &objArg,
					 const ymdhms &reference)
	: formatter(objArg, pick_short_format(objArg, reference))
{
}

#if 0
{
#endif
}
