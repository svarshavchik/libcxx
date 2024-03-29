/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ymdhms.H"
#include "x/to_string.H"
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
	tmValue.tm_mday=get_day();
	tmValue.tm_mon=get_month()-1;
	tmValue.tm_year=get_year()-1900;
	tmValue.tm_wday=get_day_of_week();
	tmValue.tm_yday= static_cast<const ymd &>(*this)
		- ymd(get_year(), 1, 1);
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

ymdhms::~ymdhms()
{
}

time_t ymdhms::to_time_t(const std::string &s,
			 const const_locale &l)
{
	auto time_get=l->get_facet<facets::time_get_facet<char> >();

	std::vector<int> numbers;

	numbers.reserve(5);
	int tz=0;
	int month=0;

	std::u32string uc;

	unicode::iconvert::convert(s, l->charset(), uc);

	for (auto b=uc.begin(), e=uc.end(); b != e;)
	{
		if (unicode_isspace(*b))
		{
			++b;
			continue;
		}

		int plusminus=0;

		if (*b == '-' || *b == '+')
		{
			plusminus= *b;

			if (++b == e)
				break;
		}

		if (unicode_isdigit(*b))
		{
			int n=0;

			int i=0;

			while (b != e && unicode_isdigit(*b))
			{
				if (++i > 4)
					goto bad;

				n = n * 10 + (*b & 0x0F);
				++b;
			}

			if (b != e)
			{
				if (*b == ':' || // Handle %H:%M:%S
				    *b == '-'   // Handle the %d in %d-%b-%Y
				    )
				{
					++b;
					plusminus=0;
				}
			}

			if (plusminus)
				tz=((n % 100) + (n / 100) * 60)
					* (plusminus == '-' ? -60:60);
			else
				numbers.push_back(n);
			continue;
		}

		auto p=b;

		while (b != e)
		{
			if (unicode_isspace(*b) || *b == '-')
				break;

			++b;
		}

		auto lower_month=unicode::iconvert::convert
			(std::u32string(p, b), l->charset());

		if (b != e && *b == '-')
		{
			// Handle the %b in %d-%b-%Y
			++b;
		}

		std::istringstream i(lower_month);

		tm tmp=tm();

		tmp.tm_mon= -1;

		std::ios_base::iostate s;

		time_get->getFacetConstRef()
			.get_monthname(std::istreambuf_iterator<char>(i),
				       std::istreambuf_iterator<char>(),
				       i, s, &tmp);
		if (tmp.tm_mon >= 0 && tmp.tm_mon < 12)
		{
			month=tmp.tm_mon+1;
		}
	}

	if (numbers.size() != 5 || month == 0)
	{
	bad:
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

std::strong_ordering ymdhms::operator<=>(const ymdhms &o) const noexcept
{
	if (getUTCoffset() != o.getUTCoffset())
		return tz->compute_time_t(*this) <=>
			tz->compute_time_t(o);

	auto c=ymd::operator<=>(o);

	if (c != std::strong_ordering::equal)
		return c;

	return hms::operator<=>(o);
}

bool ymdhms::operator==(const ymdhms &o) const noexcept
{
	if (getUTCoffset() != o.getUTCoffset())
		return tz->compute_time_t(*this) <
			tz->compute_time_t(o);

	return ymd::operator==(o) && hms::operator==(o);
}

bool ymdhms::operator!=(const ymdhms &o) const noexcept
{
	return !operator==(o);
}

ymdhms::formatter::formatter(const ymdhms &objArg)
	: obj(objArg),
	  format_string("%a, %d %b %Y %H:%M:%S %z")
{
}

ymdhms::operator std::string() const
{
	return LIBCXX_NAMESPACE::to_string(formatter(*this));
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

std::ostream &operator<<(std::ostream &o, const ymdhms &datetime)
{
	return o << (std::string)datetime;
}

#if 0
{
#endif
}
