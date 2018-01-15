/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "addoverflow.h"
#include "x/sysexception.H"
#include "x/ymd.H"
#include "x/strtok.H"
#include "x/messages.H"
#include "x/tostring.H"
#include "x/interval.H"
#include "x/strftime.H"
#include "gettext_in.h"

#include <time.h>
#include <sys/time.h>

#include <list>
#include <algorithm>
#include <limits>

#include "ymd_internal.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

const ymd::daynum_t ymd::gregorian_reformation_date_jd=

#include "ymd_grdjd.h"

	;

ymd::ymd()
{
	time_t t;
	struct tm tmval;

	if ((t=time(0)) == (time_t)-1)
		throw SYSEXCEPTION("time");

	if (!localtime_r(&t, &tmval))
		throw SYSEXCEPTION("localtime");

	year=tmval.tm_year+1900;
	month=tmval.tm_mon+1;
	day=tmval.tm_mday;

	compute_daynum();
}

ymd::ymd(const char *string, bool mdyflag, const const_locale &loc)
{
	*this= parser(loc).mdy(mdyflag).parse(string);
}

ymd::ymd(const std::string &string, bool mdyflag, const const_locale &loc)
{
	*this= parser(loc).mdy(mdyflag).parse(string);
}

ymd::~ymd()
{
}

ymd ymd::max()
{
	return ymd(EPOCHEND, 12, 31);
}

ymd::ymd(uint16_t yearArg, uint8_t monthArg, uint8_t dayArg)

{
	if (yearArg < 1 || yearArg > EPOCHEND
	    || monthArg < 1 || monthArg > 12 ||
	    dayArg < 1 || dayArg > LDOM(monthArg, yearArg) ||

	    (yearArg == gregorian_reformation_date_year &&
	     monthArg == gregorian_reformation_date_month &&
	     dayArg > gregorian_reformation_date_day &&
	     dayArg <=
	     gregorian_reformation_date_day+gregorian_reformation_days)
	    )
		throw EXCEPTION(_("Invalid date"));

	year=yearArg;
	month=monthArg;
	day=dayArg;
	compute_daynum();
}

uint16_t ymd::get_last_day_of_month() const noexcept
{
	return LDOM(month, year);
}

ymd::ymd(daynum_t dayNumber)
{
	daynum=dayNumber;

	if (dayNumber > gregorian_reformation_date_jd)
	{
		dayNumber += gregorian_reformation_days;

		if (dayNumber < gregorian_reformation_date_jd)
		{
		overflow:
			throw EXCEPTION(_("Date overflow"));
		}
	}

	// Include year 0

	dayNumber += 366;

	if (dayNumber < 366)
		goto overflow;

	daynum_t n4=(dayNumber / NDY4);

	if (n4 * 4 > gregorian_reformation_date_year)
		n4=gregorian_reformation_date_year / 4;

	year=n4 * 4;

	dayNumber -= n4 * NDY4;

	daynum_t nDaysInYear;

	while (dayNumber >= (nDaysInYear=leapyear(year) ? 366:365))
	{
		if ((year % 400) == 0)
		{
			n4 = dayNumber / NDC4;

			if (n4)
			{
				if ((daynum_t)(EPOCHEND-year) < n4*400)
					goto overflow;

				dayNumber -= n4 * NDC4;
				year += n4 * 400;
				continue;
			}
		}
		else if ((year % 100) == 0 && dayNumber >= NDCN)
		{
			if (EPOCHEND-year < 100)
				goto overflow;
			dayNumber -= NDCN;
			year += 100;
			continue;
		}

		dayNumber -= nDaysInYear;
		if (year++ == EPOCHEND)
			goto overflow;
	}

	month=1;

	uint8_t ndim;

	while (dayNumber >= (ndim=LDOM(month, year)))
	{
		dayNumber -= ndim;
		++month;
	}

	day=dayNumber+1;
}

ymd &ymd::operator+=(const interval &d_interval)
{
	sdaynum_t days=d_interval.weeks;

	if (days > std::numeric_limits<sdaynum_t>::max()/7 ||
	    days < std::numeric_limits<sdaynum_t>::min()/7)
	{
	overflow:
		throw EXCEPTION(_("Date overflow"));
	}
	days *= 7;

	if (d_interval.days > 0)
	{
		sdaynum_t n=days + d_interval.days;

		if (n < d_interval.days)
			goto overflow;
	}
	else if (d_interval.days < 0)
	{
		sdaynum_t n=days + d_interval.days;

		if (n > d_interval.days)
			goto overflow;
	}

	days += d_interval.days;

	uint16_t y=year;
	uint8_t m=month;
	uint8_t d=day;

	uint8_t ldom;

	bool wasLdom=d == LDOM(m, y);

	if (d_interval.months > EPOCHEND * 12 ||
	    d_interval.months < -EPOCHEND * 12 ||
	    d_interval.years > EPOCHEND ||
	    d_interval.years < -EPOCHEND * 12)
		goto overflow;

	sdaynum_t nMonths=d_interval.months + d_interval.years*12;

	y += nMonths / 12;
	m += nMonths % 12;

	if (m > 12)
	{
		m -= 12;
		++y;
	}

	if (m <= 0)
	{
		m += 12;
		--y;
	}

	ldom=LDOM(m, y);

	if (wasLdom || d > ldom)
		d=ldom;

	if (y == gregorian_reformation_date_year &&
	    m == gregorian_reformation_date_month &&
	    d > gregorian_reformation_date_day &&
	    d <= gregorian_reformation_date_day + gregorian_reformation_days)
	{
		if (nMonths >= 0)
		{
			d = gregorian_reformation_date_day
				+ gregorian_reformation_days + 1;
		}
		else
		{
			d = gregorian_reformation_date_day;
		}
	}

	while (days > 0 && days < 1000)
	{
		ldom=LDOM(m, y);

		if (d >= ldom)
		{
			d=1;
			if (++m > 12)
			{
				m=1;
				if (y++ >= EPOCHEND)
					goto overflow;
			}
			--days;
			continue;
		}

		if (y == gregorian_reformation_date_year &&
		    m == gregorian_reformation_date_month)
		{
			if (d++ == gregorian_reformation_date_day)
				d += gregorian_reformation_days;
			--days;
			continue;
		}

		sdaynum_t n=ldom-d;

		if (n > days)
			n=days;

		d += n;
		days -= n;
	}

	while (days < 0 && days > -1000)
	{
		if (d == 1)
		{
			if (--m <= 0)
			{
				m=12;
				if (--y <= 0)
					goto overflow;
			}

			d=LDOM(m, y);
			++days;
			continue;
		}

		if (y == gregorian_reformation_date_year &&
		    m == gregorian_reformation_date_month)
		{
			if (--d == gregorian_reformation_date_day +
			    gregorian_reformation_days)
				d= gregorian_reformation_date_day;
			++days;
			continue;
		}

		sdaynum_t n= -days;

		if (n >= d)
			n=d-1;

		d -= n;
		days += n;
	}

	if (days == 0)
	{
		year=y;
		month=m;
		day=d;
		compute_daynum();
		return *this;
	}

	daynum_t j=jd();

	daynum_t jj=j+days;

	if (days < 0)
	{
		if (jj > j)
			goto overflow;
	}
	else
	{
		if (jj < j)
			goto overflow;
	}

	return *this=ymd(jj);
}

ymd::sdaynum_t ymd::operator-(const ymd &other) const noexcept
{
	daynum_t a=jd();
	daynum_t b=other.jd();

	if (a > b)
		return a-b;

	return -(b-a);
}


ymd::interval::interval(const std::string &sArg,
			const const_locale &localeArg)
	: years(0), months(0), weeks(0), days(0)
{
	LIBCXX_NAMESPACE::interval<sdaynum_t> parser(interval_descr, 0,
						     libmsg());

	const std::vector<sdaynum_t> &vec=parser.parse(sArg, localeArg);

	days=vec[0];
	weeks=vec[1];
	months=vec[2];
	years=vec[3];
}

ymd::interval ymd::interval::operator-() const
{
	interval n;

	n.years= -years;
	n.months= -months;
	n.weeks= -weeks;
	n.days= -days;

	if ((years && n.years == years) ||
	    (months && n.months == months) ||
	    (weeks && n.weeks == weeks) ||
	    (days && n.days == days))
		throw EXCEPTION(_("Date interval overflow"));

	return n;
}

ymd::interval &ymd::interval::operator+=(const ymd::interval &i)

{
	sdaynum_t y, m, w, d;

	if (addchkoverflow(y, years, i.years) ||
	    addchkoverflow(m, months, i.months) ||
	    addchkoverflow(w, weeks, i.weeks) ||
	    addchkoverflow(d, days, i.days))
		throw EXCEPTION(_("Date interval overflow"));

	years=y;
	months=m;
	weeks=w;
	days=d;

	return *this;
}

const char * const ymd::interval::interval_descr[]={_txtn("day", "days"),
						    _txtn("week", "weeks"),
						    _txtn("month", "months"),
						    _txtn("year", "years"),
						    0};

std::string ymd::interval::internal_tostring(const const_locale &l) const
{
	messages msgcat(messages::create(l, LIBCXX_DOMAIN));

	std::ostringstream o;

	const char *sep="";

	if (years)
	{
		o << msgcat->formatn(_txtn("%1% year", "%1% years"),
				     years, years);
		sep=", ";
	}

	if (months)
	{
		o << sep
		  << msgcat->formatn(_txtn("%1% month", "%1% months"),
				     months, months);
		sep=", ";
	}

	if (weeks)
	{
		o << sep
		  << msgcat->formatn(_txtn("%1% week", "%1% weeks"),
				     weeks, weeks);
		sep=", ";
	}

	if (days || (!weeks && !months && !years))
	{
		o << sep
		  << msgcat->formatn(_txtn("%1% day", "%1% days"),
				     days, days);
		sep=", ";
	}

	return o.str();
}

ymd::interval::operator std::string() const
{
	return internal_tostring();
}

ymd::iso8601::iso8601(const ymd &date)
	: year(date.get_year())
{
	sdaynum_t dayCount;

	if (date.get_month() == 12)
	{
		ymd firstDay(ymd::firstDayOfWeek(year+1, 4)-3);

		if (firstDay <= date)
		{
			++year;

			dayCount=date - firstDay;
			goto cleanup;
		}

	}

	{
		ymd firstDay(ymd::firstDayOfWeek(year, 4)-3);

		if (firstDay > date)
		{
			firstDay=ymd::firstDayOfWeek(--year, 4);
			firstDay -= 3;
		}
		dayCount=date - firstDay;
	}

 cleanup:
	week= (dayCount / 7)+1;
	daynum = (dayCount % 7)+1;
}

ymd::iso8601::iso8601(uint16_t yearArg,
		      int weekArg,
		      int dayArg)
{
	if (yearArg < 1 || yearArg > EPOCHEND ||
	    weekArg < 1 || weekArg > 53 ||
	    dayArg < 1 || dayArg > 7)
		throw EXCEPTION(_("Invalid ISO 8601 date"));

	year=yearArg;
	week=weekArg;
	daynum=dayArg;
}

ymd::iso8601::~iso8601()
{
}

std::string ymd::iso8601::toString() const
{
	char d[2];
	char dw[3];

	d[0]=dw[0]='-';
	dw[1]='W';
	d[1]=dw[2]=0;

	std::ostringstream o;

	imbue<std::ostringstream> force_c(locale::create("C"), o);

	o << std::setw(4) << std::setfill('0') << year << dw
	  << std::setw(2) << std::setfill('0') << (int)week << d
	  << std::setw(1) << (int)daynum;
	return o.str();
}

ymd::ymd(const ymd::iso8601 &iso8601Date)
{
	ymd firstDay(firstDayOfWeek(iso8601Date.get_year(), 4));

	firstDay -= 3;

	interval ival;

	ival.weeks=iso8601Date.get_week()-1;
	ival.days=iso8601Date.get_day_of_week()-1;

	*this= firstDay + ival;
}

ymd::parser::parser(const const_locale &locArg)
	: loc(locArg), usmdy(false)
{
	char fmtbuf[3];

	fmtbuf[0]='%';
	fmtbuf[2]=0;

	auto utf8=locale::base::utf8();

	for (size_t i=0; i<12; i++)
	{
		ymd day(jan1suyear, 1+i, 1);

		fmtbuf[1]='b';

		unicode::iconvert::convert(day.format_date(fmtbuf, utf8),
					   unicode::utf_8,
					   month_names[i]);

		fmtbuf[1]='B';

		unicode::iconvert::convert(day.format_date(fmtbuf, utf8),
					   unicode::utf_8,
					   month_names_long[i]);

		month_names[i]=unicode::toupper(month_names[i]);
		month_names_long[i]=unicode::toupper(month_names_long[i]);
	}

	auto time_get=locArg->get_facet<facets::time_get_facet<char> >();

	if (time_get->getFacetConstRef().date_order()==std::time_base::mdy)
		usmdy=true;
}

ymd::parser::~parser()
{
}

ymd ymd::parser::parse(const char *string)
{
	size_t n;

	for (n=0; string[n]; ++n)
		;

	return parse(string, string+n);
}

ymd ymd::parser::parse_internal(const std::u32string &ustr)
{
	int md[2];
	int md_cnt=0;

	bool md_str=false;
	std::u32string mstr;

	uint16_t year=0;
	bool yearfound=false;

	bool yearfirst=false;
	bool first=true;

	size_t current_number=0;
	int in_number=0;

	auto b=ustr.begin(), e=ustr.end();

	// Parse numbers we see, maybe a month name.

	while (1)
	{
		if (b != e && unicode_isdigit(*b))
		{
			// Seeing a digit, add it to current_number

			current_number=current_number * 10 + (*b & 0x0F);

			// Can't see 5 or more consecutive digits.

			if (++in_number > 4)
				goto bad;
			++b;
			continue;
		}

		if (in_number)
		{
			// Something that's not a digit, after a digit

			if (in_number == 4)
			{
				// Saw four digits, must be a year.

				if (yearfound)
					goto bad;

				yearfound=true;
				yearfirst=first;
				year=current_number;
			}
			else
			{
				// Can't see three or more numbers

				if (md_cnt >= 2)
					goto bad;

				if ((uint8_t)current_number != current_number)
					goto bad;

				md[md_cnt]=current_number;
				++md_cnt;
			}
			current_number=0;
			in_number=0;
			first=false;
		}

		if (b == e)
			break;

		if (!unicode_isalpha(*b))
		{
			++b;
			continue;
		}

		// Seeing letters, must be a month name

		if (md_str)
			goto bad;

		while (b != e && unicode_isalnum(*b))
		{
			mstr.push_back(*b);
			++b;
		}

		md_str=true;
	}

	// Now, figure out what we have.

	uint16_t month, day;

	if (!yearfound)
		goto bad;

	if (md_str)
	{
		if (md_cnt != 1)
			goto bad;

		if (mstr.size() > 1 && mstr[0] == 'W')
		{
			// W<week number>

			if (std::find_if(mstr.begin()+1,
					 mstr.end(),
					 []
					 (char32_t uc)
					 {
						 return !unicode_isdigit(uc);
					 })
			    == mstr.end())
			{
				std::transform(mstr.begin()+1,
					       mstr.end(),
					       mstr.begin()+1,
					       []
					       (char32_t uc)
					       {
						       return (uc & 0x0f) + '0';
					       });


				std::istringstream i(std::string(mstr.begin()+1,
								 mstr.end()));

				uint16_t n=0;

				i >> n;

				if (!i.fail())
					return ymd(ymd::iso8601(year, n,
								md[0]));
			}
		}

		// Convert to uppercase unicode, match to a month.

		mstr=unicode::toupper(mstr);

		for (month=0; month<12; month++)
			if (month_names[month] == mstr ||
			    month_names_long[month] == mstr)
				break;

		if (month >= 12)
			goto bad;
		++month;
		day=md[0];
	}
	else
	{
		if (md_cnt != 2)
		{
		bad:

			throw EXCEPTION(_("Date parsing failed"));
		}

		if (yearfirst || usmdy)
		{
			month=md[0];
			day=md[1];
		}
		else
		{
			day=md[0];
			month=md[1];
		}
	}

	return ymd(year, month, day);
}

std::string ymd::format_date(const char *pattern,
			    const const_locale &localeRef)
	const
{
	static const char isodate[]="%d-%b-%Y";

	if (!pattern)
		pattern=isodate;

	return strftime(*this, localeRef)(pattern);
}

std::string ymd::format_date(const std::string &pattern,
			    const const_locale &localeRef)
	const
{
	return format_date(pattern.c_str(), localeRef);
}

ymd::operator std::string() const
{
	return format_date();
}

#ifndef DOXYGEN
template ymd ymd::parser::parse(const char *, const char *);
#endif

#if 0
{
#endif
}
