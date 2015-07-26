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

time_t ymdhms::do_to_time_t(const function<bool ()> &eof,
			    const function<char ()> &curchar,
			    const function<void ()> &nextchar,
			    const const_locale &l)
{
	auto time_get=l->get_facet<facets::time_get_facet<char> >();

	ctype ct(l);

	std::vector<int> numbers;

	numbers.reserve(5);
	int tz=0;
	int month=0;

	while (!eof())
	{
		auto c=curchar();

		if (ct.is(c, std::ctype_base::space))
		{
			nextchar();
			continue;
		}

		int plusminus=0;

		if (c == '-' || c == '+')
		{
			plusminus=c;

			nextchar();

			if (eof())
				break;
			c=curchar();
		}

		if (c >= '0' && c <= '9')
		{
			int n=0;

			while (!eof() && (c=curchar()) >= '0' && c <= '9')
			{
				n = n * 10 + (c-'0');
				nextchar();
			}

			c=curchar();
			if (!eof())
			{
				if (c == ':')
					nextchar();   // Handle %H:%M:%S
				else if (c == '-')
					nextchar();   // Handle the %d in %d-%b-%Y
			}

			if (plusminus)
				tz=((n % 100) + (n / 100) * 60)
					* (plusminus == '-' ? -60:60);
			else
				numbers.push_back(n);
			continue;
		}

		std::stringstream ss;

		while (!eof())
		{
			c=curchar();

			if (ct.is(c, std::ctype_base::space))
				break;

			if (c == '-')
			{
				// Handle the %b in %d-%b-%Y
				nextchar();
				break;
			}
			ss << c;
			nextchar();
		}

		tm tmp=tm();

		tmp.tm_mon= -1;

		std::ios_base::iostate s;

		time_get->getFacetConstRef()
			.get_monthname(std::istreambuf_iterator<char>(ss),
				       std::istreambuf_iterator<char>(),
				       ss, s, &tmp);
		if (tmp.tm_mon >= 0 && tmp.tm_mon < 12)
		{
			month=tmp.tm_mon+1;
		}
	}

	if (numbers.size() != 5 || month == 0)
	{
		throw EXCEPTION(libmsg(_txt("Cannot parse time value")));
	}

	auto year=numbers[1];
	auto day=numbers[0];

	auto hour=numbers[2];
	auto minute=numbers[3];
	auto second=numbers[4];

	if (second > 1900)
	{
		// "%a %b %d %H:%M:%S %Y".

		auto year_save=second;

		second=minute;
		minute=hour;
		hour=year;
		year=year_save;
	}

	if (year < 50)
		year += 2000;
	if (year < 100)
		year += 1900;

	return (time_t)ymdhms( ymd(year, month, day),
			       hms(hour, minute, second),
			       tzfile::base::utc())
		- tz;
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

ymdhms::formatter::formatter(const ymdhms &objArg)
	: obj(objArg),
	  format_string("%a, %d %b %Y %H:%M:%S %z")
{
}

ymdhms::operator std::string() const
{
	return tostring(formatter(*this));
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
