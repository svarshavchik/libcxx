/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "addoverflow.h"
#include "x/hms.H"
#include "x/strtok.H"
#include "x/messages.H"
#include "x/interval.H"
#include "x/strftime.H"
#include "gettext_in.h"

#include <cstring>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <sys/time.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

hms::hms(const ymd::interval &ymd_interval)
{
	if (ymd_interval.months || ymd_interval.years)
		throw EXCEPTION(_("Interval cannot specify months or years"));

	ymd::sdaynum_t hv=0, mv=0, sv=0;

	normalize(hv, mv, sv, ymd_interval.days, ymd_interval.weeks);

	if (hv != (int32_t)hv)
	{
		throw EXCEPTION(_("Time interval overflow"));
	}

	h=hv;
	m=mv;
	s=sv;
}

hms::hms(time_t seconds)
{
	time_t th=0;
	time_t tm=0;

	normalize(th, tm, seconds, time_t(0), time_t(0));

	if (th != (int32_t)th)
	{
		throw EXCEPTION(_("Time interval overflow"));
	}

	h=th;
	m=tm;
	s=seconds;
}

hms::hms(const std::string &intervalStr,
	 const const_locale &l)
{
	construct(intervalStr.begin(), intervalStr.end(), l);
}

hms::hms(const char *intervalStr,
	 const const_locale &l)
{
	std::string s(intervalStr);

	construct(s.begin(), s.end(), l);
}

template void hms::construct(std::string::const_iterator,
			     std::string::const_iterator,
			     const const_locale &);

template<typename int_type>
void hms::normalize(//! Hours
		    int_type &hv,

		    //! Minutes
		    int_type &mv,

		    //! Seconds
		    int_type &sv,

		    //! days
		    int_type days,

		    //! weeks
		    int_type weeks)
{


	int_type wd=weeks;

	int_type wn=wd * 7;

	int_type wh;

	if (wn / 7 != wd || (wn % 7) ||
	    addchkoverflow(wd, wn, days) ||
	    (wn=wd * 24) / 24 != wd ||
	    (wn % 24) ||
	    addchkoverflow(wh, hv, wn))
	{
	overflow:
		throw EXCEPTION(_("Time interval overflow"));
	}

	int_type mm;
	int_type so=sv/60;

	if (addchkoverflow(mm,mv,so))
		goto overflow;

	sv %= 60;

	int_type hh;
	int_type mo=mm/60;

	if (addchkoverflow(hh,wh,mo))
		goto overflow;

	mm %= 60;

	hv=hh;
	mv=mm;

	if (mv < 0 && hv > 0)
	{
		mv += 60;
		--hv;
	}

	if (mv > 0 && hv < 0)
	{
		mv -= 60;
		++hv;
	}

	if (sv < 0 && (mv > 0 || hv > 0))
	{
		sv += 60;

		if (--mv < 0)
		{
			mv += 60;
			--hv;
		}
	}

	if (sv > 0 && (mv < 0 || hv < 0))
	{
		sv -= 60;

		if (++mv > 0)
		{
			mv -= 60;
			++hv;
		}
	}
}

hms hms::operator-() const
{
	int32_t nh(-h);
	int32_t nm(-m);
	int32_t ns(-s);

	if ((h && h == nh) ||
	    (m && m == nm) ||
	    (s && s == ns))
		throw EXCEPTION(_("Time interval overflow"));

	return hms(nh, nm, ns);
}

hms &hms::operator+=(const hms &o)
{
	int32_t nh;
	int32_t nm;
	int32_t ns;

	if (addchkoverflow(nh, h, o.h) ||
	    addchkoverflow(nm, m, o.m) ||
	    addchkoverflow(ns, s, o.s))
		throw EXCEPTION(_("Time interval overflow"));

	normalize(nh, nm, ns, 0, 0);

	h=nh;
	m=nm;
	s=ns;
	return *this;
}

time_t hms::seconds() const
{
	int64_t sv=h;
	int64_t ss;

	ss= sv * 60;

	if (ss / 60 != sv)
	{
	overflow:
		throw EXCEPTION(_("Time interval overflow"));
	}

	if (addchkoverflow(sv, ss, m))
		goto overflow;

	ss= sv * 60;

	if (ss / 60 != sv)
		goto overflow;

	if (addchkoverflow(sv, ss, s))
		goto overflow;

	time_t t=(time_t)sv;

	if (t < 0 || (int64_t)t != sv)
		goto overflow;

	return t;
}

const char * const hms::interval_descr[]={_txtn("hour", "hours"),
					  _txtn("minute", "minutes"),
					  _txtn("second", "seconds"),
					  _txtn("day", "days"),
					  _txtn("week", "weeks"),
					  0};

std::string hms::toIntervalString(const const_locale &localeRef)
	const
{
	std::ostringstream o;

	if (h)
		o << gettextmsg(_TXTN("%1% hour", "%1% hours", h), h);


	if (m)
	{
		if (h)
			o << ", ";

		o << gettextmsg(_TXTN("%1% minute", "%1% minutes", m), m);
	}

	if (s)
	{
		if (h || m)
			o << ", ";

		o << gettextmsg(_TXTN("%1% second", "%1% seconds", s), s);
	}

	return o.str();
}

void hms::parsing_error()
{
	throw EXCEPTION(_("Invalid time interval specification"));
}

std::string hms::verboseString(const const_locale &l) const
{
	auto msgs=messages::create(l, LIBCXX_DOMAIN);

	std::string hours_str, minute_str, seconds_str;

	{
		std::ostringstream o;

		o << gettextmsg(msgs->get(_txtn("%1% hour",
						"%1% hours"), h), h);

		hours_str=o.str();
	}

	{
		std::ostringstream o;

		o << gettextmsg(msgs->get(_txtn("%1% minute",
						"%1% minutes"), m), m);

		minute_str=o.str();
	}

	{
		std::ostringstream o;

		o << gettextmsg(msgs->get(_txtn("%1% second",
						"%1% seconds"), s), s);

		seconds_str=o.str();
	}

	std::ostringstream o;

	const char sepzero=0;
	const char sepcomma[]=", ";

	const char *sep=&sepzero;

	if (h)
	{
		o << hours_str;
		sep=sepcomma;
	}

	if (m)
	{
		o << sep << minute_str;
		sep=sepcomma;
	}

	if (s || *sep == 0)
	{
		o << sep << seconds_str;
	}

	return o.str();
}

std::string hms::hhmmss(const char *pattern,
			const const_locale &localeRef)
	const
{
	const char def_time[]="%X";

	if (!pattern)
		pattern=def_time;

	return strftime(*this, localeRef)(pattern);
}

std::string hms::hhmmss(const std::string &pattern,
			const const_locale &localeRef) const
{
	return hhmmss(pattern.c_str(), localeRef);
}

#if 0
{
#endif
}
