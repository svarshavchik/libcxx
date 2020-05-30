/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ymdhmsfwd_H
#define x_ymdhmsfwd_H

#include <x/ymdhmsbase.H>
#include <x/tzfile.H>
#include <x/locale.H>
#include <x/facet.H>
#include <iostream>
#include <type_traits>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A date and time

//! This object represents a date and time in a local timezone. This
//! is an expensive object, and it's always better to use \c time_t to
//! represent a timezone-independent calendar time, converting to a ymdhms
//! object when it's necessary to express it as local time.
//!
//! ymdhms carries the same general kind of information as the \c tm structure
//! described in the \c localtime(3) man page, but offers more features.
//! A ymdhms object can be converted directly to a \c tm structure.
//!
//! Although this class defines comparison operators, and these objects may
//! be placed in associative containers, a date/time comparison is expensive,
//! due to timezone-related issues; while comparing \c time_t values is cheap.
//!
//! \par Converting calendar time to local time
//!
//! \code
//! time_t t=time(NULL);
//!
//! ymdhms now(t, tzfile::base::local());
//!
//! ymd now_date(now);
//! hms now_time(now);
//!
//! struct tm tmstruct=now;
//!
//! \endcode
//!
//! This constructor takes a \c time_t and instantiates an ymdhms object
//! representing the given date and time in the given timezone. This object
//! can subsequently be decomposed into separate ymd and hms objects, or
//! converted to a C library compatible \c tm structure.
//!
//! Alternatively, ymd's and hms's methods and members can be accessed directly:
//!
//! \code
//! uint16_t year=now.get_year();
//! uint16_t month=now.get_month();
//! uint16_t day=now.get_day();
//!
//! int32_t h=now.h;
//! int32_t m=now.m;
//! int32_t s=now.s;
//! \endcode
//!
//! \note
//! The second value can be set to 60, in timezones that use leap seconds.
//!
//! Additional methods:
//!
//! \code
//! int32_t offset=now.getUTCoffset();
//! std::string name=now.getTzname();
//! \endcode
//!
//! getUTCoffset() returns the offset, in seconds, of the local timezone from
//! \c UTC, and getTzname() returns the abbreviation of the local timezone.
//!
//! \par Converting calendar time to a different timezone
//!
//! \code
//! ymdhms nyc( time(NULL), tzfile::create("America/New_York");
//!
//! // ...
//!
//! ymdhms la(nyc, tzfile::create("America/Los_Angeles"));
//! \endcode
//!
//! An ymdhms object may be constructed using another ymdhms and a timezone
//! reference. The end result is the specified date and time converted to a
//! different timezone.
//!
//! This is an expensive conversion. Where possible, it's faster to maintain
//! \c time_t calendar time, and converting to calendar time, as needed.
//!
//! \par Converting local time to calendar time
//!
//! \code
//! time_t timeValue=now;
//! \endcode
//!
//! This is the reverse operation: converting a ymdhms date and time to the
//! \c time_t calendar time.
//!
//! An ymdhms object can also be constructed from individual ymd and hms
//! objects, using a given timezone specification:
//!
//! \code
//! tzfile nyc(tzfile::create("America/New_York"));
//!
//! ymd dateValue(2007, 11, 4);
//! hms timeValue(1, 30, 0);
//!
//! ymdhms datetime(dateValue, timeValue, nyc);
//!
//! time_t dateTimeValue=datetime;
//! \endcode
//!
//! This is an example of how to convert a date and a time to a \c time_t,
//! in a specific timezone. Note that this example, of 01:30 AM on Nov 4,
//! 2007 can be either standard time or daylight savings time.
//!
//! Note that
//! when a given date and time may be ambiguous due to timezone changes,
//! as is the case here, it is unspecified which timezone the resulting
//! ymdhms object refers to.


class ymdhms : public ymdhmsBase {

	//! Governing timezone file.

	const_tzfile tz;

public:
	//! Construct a ymdhms object for given calendar time

	ymdhms(//! Calendar time
	       time_t timevalue=time(0),

	       //! Timezone specification

	       const const_tzfile &tzfileArg=tzfile::base::local());

	//! Construct a ymdhms object for the same calendar time in a different timezone

	ymdhms(//! Calendar time
	       const ymdhms &timevalue,

	       //! New timezone

	       const const_tzfile &tzfileArg);

	//! Construct a ymdhms object from a discrete date and time

	ymdhms(//! The date
	       const ymd &ymdVal,

	       //! The time of day
	       const hms &hmsVal,

	       //! Timezone specification

	       const const_tzfile &tzfileArg=tzfile::base::local());

	//! Construct a ymdhms object from a discrete date and time

	ymdhms(//! The date ad time
	       const struct tm &timeValue,

	       //! Timezone specification
	       const const_tzfile &tzfileArg=tzfile::base::local());

	//! Convert a string to a time_t

	//! The string is expected to be formatted using any of the three
	//! formats specified in section 3.3 of RFC 2616, namely:
	//!
	//! - "%a, %d %b %Y %H:%M:%S %z".
	//! - "$A, %d-%b-%Y %H:%M:%S %z".
	//! - "%a %b %d %H:%M:%S %Y".
	//!
	//! An exception gets thrown if the string cannot be parsed, as such.

	static time_t to_time_t(const std::string &s,
				const const_locale &l);

	//! Instantiate from a string

	//! The string is expected to be formatted using any of the three formts
	//! specified in section 3.3 of RFC 2616, namely:
	//!
	//! - "%a, %d %b %Y %H:%M:%S %z".
	//! - "$A, %d-%b-%Y %H:%M:%S %z".
	//! - "%a %b %d %H:%M:%S %Y".
	//!
	//! An exception gets thrown if the string cannot be parsed, as such.

	explicit ymdhms(//! Formatted string
			const std::string &timeValue,

			//! Timezone specification
			const const_tzfile &tzfileArg=tzfile::base::local(),

			//! Locale for parsing the month name
			const const_locale &localeRef=locale::base::utf8())
		: ymdhmsBase(tzfileArg->compute_ymdhms
			     (to_time_t(timeValue, localeRef))),
		  tz(tzfileArg)
	{
	}

	//! Default destructor
	~ymdhms();

	//! Retrieve the effective timezone

	const const_tzfile &getTimezone() const noexcept
	{
		return tz;
	}

	//! Helper class used by format()
	class formatter {

	public:

		//! The date/time
		const ymdhms &obj;

		//! Format string to use
		const std::string format_string;

		//! Constructor
		inline formatter(const ymdhms &objArg,
				 const std::string &format_stringArg)
			: obj(objArg), format_string(format_stringArg)
		{
		}

		//! Constructor, default date time format
		formatter(const ymdhms &objArg);

		//! Destructor
		inline ~formatter() {}

		//! Convert to a string
		template<typename OutputIterator>
		inline OutputIterator to_string(//! Output iterator
					       OutputIterator iter,

					       //! Locale
					       const const_locale &localeRef
					       =locale::base::global()) const;
	};

	//! Helper class used by short_format()

	class short_formatter : public formatter {

	public:
		//! Constructor

		short_formatter(const ymdhms &objArg);

		//! Constructor
		short_formatter(const ymdhms &objArg,
				const ymdhms &reference);

		//! Destructor
		inline ~short_formatter() {}
	};

	//! Format this date/time object using a short string

	//! For date/times within the last 6 months, use
	//! "%b %d %H:%M", otherwise "%b %d  %Y".

	inline short_formatter short_format() const
	{
		return short_formatter(*this);
	}

	//! Format this date/time object using a short string

	//! \overload
	inline short_formatter short_format(const ymdhms &reference) const
	{
		return short_formatter(*this, reference);
	}

	//! Format this date/time as a string, using a custom format

	template<typename format_type>
	inline formatter format() const
	{
		return formatter(*this);
	}

	//! Format this date/time as a string, using a custom format

	inline formatter format(const std::string &format_type_arg) const
	{
		return formatter(*this, format_type_arg);
	}

	//! Convert this date and time value to a string

	operator std::string() const;

	//! Convert this date/time value back to calendar time.

	operator time_t() const
	{
		return tz->compute_time_t(*this);
	}


	//! Convert his date/time value to a \c tm structure instance.

	//! \note
	//! The returned \c tm structure is valid only as long as this
	//! ymdhms object is in scope, because the timezone name pointer
	//! points back to this object.

	operator struct tm() const;

	//! Comparison operator

	bool operator<(const ymdhms &o) const;

	//! Comparison operator

	bool operator>(const ymdhms &o) const;

	//! Comparison operator

	bool operator==(const ymdhms &o) const;

	//! Comparison operator

	bool operator!=(const ymdhms &o) const;

	//! Comparison operator

	bool operator<=(const ymdhms &o) const;

	//! Comparison operator

	bool operator>=(const ymdhms &o) const;

	//! Helper used by serialize()

	template<typename T>
	inline T &ymd_hms()
	{
		return static_cast<T &>(*this);
	}

	//! Helper used by serialize()

	template<typename T>
	inline const T &ymd_hms() const
	{
		return static_cast<const T &>(*this);
	}

	//! Serialization function

	template<typename ptr_type, typename iter_type>
	static void serialize(ptr_type *ptr, iter_type &i)
	{
		i(ptr->template ymd_hms<ymd>());
		i(ptr->template ymd_hms<hms>());

		std::string name=ptr->tz->getName();

		i(name);

		if constexpr (!i.serializing)
			*ptr=ymdhms(*ptr, *ptr, tzfile::create(name));
	}
};

#if 0
{
#endif
}
#endif
