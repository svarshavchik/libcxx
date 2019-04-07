/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ymd.H"
#include "ymd_internal.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

ymd::daynum_t ymd::compute_daynum(//! Year
				  uint16_t year,

				  //! Month
				  uint8_t month,

				  //! Day of the month
				  uint8_t day)
{
	uint16_t y=year-1;

	y /= 4;

	daynum_t d=y * NDY4;

	y *= 4;

	daynum_t first_gregorian_century=
		(gregorian_reformation_date_year/100+1)*100;

	if (y > first_gregorian_century)
		d -= (y-first_gregorian_century+99)/100;

	daynum_t first_gregorian_century4=
		(gregorian_reformation_date_year/400+1)*400;

	if (y > first_gregorian_century4)
		d += (y-first_gregorian_century4+399)/400;

	if ( year > gregorian_reformation_date_year ||
	     (year == gregorian_reformation_date_year &&
	      (month > gregorian_reformation_date_month ||
	       (month == gregorian_reformation_date_month &&
		day > gregorian_reformation_date_day))))
		d -= gregorian_reformation_days;

	while (y < year)
	{
		if (leapyear(y))
		{
			d += 366;
		}
		else
		{
			d += 365;
		}
		++y;
	}

	for (uint8_t m=1; m < month; ++m)
	{
		d += LDOM(m, y);
	}

	d += (day-1);

	// All of the above counted year 0

	return d-366;
}

#if 0
{
#endif
}
