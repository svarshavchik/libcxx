/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ymdhmsbase.H"
#include "x/ymd.H"
#include "x/hms.H"
#include "x/messages.H"
#include "gettext_in.h"
#include <iomanip>
#include <x/exception.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

// epoch is accessed by logger, for log message formatting. This cannot be
// a static member because all static objects may not've been initialized, yet.

ymd ymdhmsBase::epoch() noexcept
{
	return ymd(1970, 1, 1);
}

void ymdhmsBase::internal_overflow() const
{
	std::ostringstream o;

	o << getYear() << "-" << std::setw(2) << std::setfill('0')
	  << getMonth() << "-" << std::setw(2) << std::setfill('0')
	  << getDay();

	throw EXCEPTION(gettextmsg(libmsg(_txt("Time arithmetic overflow for %1% %2%")),
				   o.str(), hms::hhmmss()));
}

time_t ymdhmsBase::start_of_day_time_t_utc() const
{
	ymd::sdaynum_t ndays=ymd::operator-(epoch());
	time_t nsecs= (time_t)ndays * (24 * 60 * 60);

	if (nsecs / (24 * 60 * 60) != ndays || nsecs % (24 * 60 * 60))
		internal_overflow();

	return nsecs;
}
#if 0
{
#endif
}
