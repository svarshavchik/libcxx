/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ymdhmsbase_H
#define x_ymdhmsbase_H

#include <x/ymdfwd.H>
#include <x/hmsfwd.H>

#include <stdint.h>

#include <string>

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! ymdhms base class

//! This class implements most of the functionality of ymdhms, which derives
//! from this class.
//!
//! \see ymdhms

class ymdhmsBase : public ymd, public hms {

	//! Saved UTC offset

	int32_t gmtoffset;

	//! Alternate timezone flag

	bool altzone;

	//! Saved timezone name

	std::string tzname;

public:

	//! Midnight, January 1 1970

	static ymd epoch() noexcept;

	//! Explicit constructor

	ymdhmsBase(//! The date component
		   const ymd &ymdArg,

		   //! The time component
		   const hms &hmsArg,

		   //! Offset from UTC

		   int32_t gmtoffsetArg,

		   //! Alternate timezone

		   bool altzoneArg,
		   //! Timezone abbreviation

		   const std::string &tznameArg) noexcept
		: ymd(ymdArg), hms(hmsArg), gmtoffset(gmtoffsetArg),
		  altzone(altzoneArg),
		  tzname(tznameArg)
	{
	}

	//! Default destructor
	~ymdhmsBase()
	{
	}

	//! Retrieve the timezone abbreviation

	//! The timezone abbreviation may not always be available; specifically
	//! when ymdhms is constructed from an explicit date, time, and a
	//! timezone offset value, rather than from one of the system-defined
	//! timezone database entries, that include timezone abbreviations.

	const std::string &getTzname() const { return tzname; }

	//! Retrieve the offset from UTC

	int32_t getUTCoffset() const { return gmtoffset; }

	//! Retrieve the alternate timezone flag

	//! \return \c true if alternate timezone (summer time) is in effect.

	bool getAltzone() const { return altzone; }

	//! Calculate the time_t for midnight of the date component, UTC.

	//! \internal
	//!
	time_t start_of_day_time_t_utc() const;

	//! Report an internal calculation overflow

	//! \internal
	//!
	void internal_overflow()
		const __attribute__((noreturn));
};

#if 0
{
#endif
}
#endif
