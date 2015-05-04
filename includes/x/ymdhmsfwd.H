/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ymdhmsfwd_H
#define x_ymdhmsfwd_H

#include <x/ymdhmsbase.H>
#include <x/tzfile.H>
#include <x/locale.H>
#include <x/facet.H>
#include <x/ctype.H>
#include <x/value_stringable.H>
#include <x/string_argument.H>
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
//! uint16_t year=now.getYear();
//! uint16_t month=now.getMonth();
//! uint16_t day=now.getDay();
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

	template<typename iter_type>
	static time_t to_time_t(iter_type b, iter_type e,
				const const_locale &l);

	//! Throw an %exception, the date and time cannot be parsed from a string.

	static void cannot_parse_time() __attribute__((noreturn));

	//! Instantiate from a string

	//! The string is expected to be formatted using any of the three formts
	//! specified in section 3.3 of RFC 2616, namely:
	//!
	//! - "%a, %d %b %Y %H:%M:%S %z".
	//! - "$A, %d-%b-%Y %H:%M:%S %z".
	//! - "%a %b %d %H:%M:%S %Y".
	//!
	//! An exception gets thrown if the string cannot be parsed, as such.

	template<typename char_type>
	ymdhms(//! Formatted string
	       const std::basic_string<char_type> &timeValue,

	       //! Timezone specification
	       const const_tzfile &tzfileArg=tzfile::base::local(),

	       //! Locale for parsing the month name
	       const const_locale &localeRef=locale::base::utf8())
		: ymdhmsBase(tzfileArg->compute_ymdhms
			     (to_time_t(timeValue.begin(), timeValue.end(),
					localeRef))),
		  tz(tzfileArg)
	{
	}

	//! Default destructor
	~ymdhms() noexcept;

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
		inline ~formatter() noexcept {}

		//! This class implements both toString and toWideString
		static const stringable_t stringable=class_toboth;

		//! Convert to a string
		template<typename OutputIterator>
		inline OutputIterator toString(//! Output iterator
					       OutputIterator iter,

					       //! Locale
					       const const_locale &localeRef
					       =locale::base::global()) const;

		//! Convert to a wide string
		template<typename OutputIterator>
		inline OutputIterator toWideString(//! Output iterator
						   OutputIterator iter,

						   //! Locale
						   const const_locale &localeRef
						   =locale::base::global())
			const;
	};

	//! Helper class used by short_format()

	class short_formatter : public formatter {

	public:
		//! Constructor

		short_formatter(const ymdhms &objArg);
		short_formatter(const ymdhms &objArg,
				const ymdhms &reference);
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

	template<typename format_type>
	inline formatter format(const format_type &format_type_arg) const
	{
		return formatter(*this, basic_string_argument(format_type_arg));
	}

	//! Convert this date and time value to a string

	operator std::string() const;

	//! Convert this date and time value to a wide string

	operator std::wstring() const;

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

	//! Serialization function

	template<typename iter_type>
	void serialize(iter_type &i)
	{
		i(static_cast<ymd &>(*this));
		i(static_cast<hms &>(*this));

		std::string name;

		name=tz->getName();

		i(name);

		if (!i.serializing)
			*this=ymdhms(*this, *this, tzfile::create(name));
	}
};

#ifndef DOXYGEN_FIXME

template<typename iter_type>
time_t ymdhms::to_time_t(iter_type b, iter_type e,
			 const const_locale &l)
{
	typedef typename std::decay<decltype(*b)>::type char_t;

	auto time_get=l->get_facet<facets::time_get_facet<char_t> >();

	basic_ctype<char_t> ct(l);

	std::vector<int> numbers;

	numbers.reserve(5);
	int tz=0;
	int month=0;

	while (b != e)
	{
		if (ct.is(*b, std::ctype_base::space))
		{
			++b;
			continue;
		}

		int plusminus=0;

		if (*b == '-' || *b == '+')
		{
			plusminus=*b;

			if (++b == e)
				break;
		}

		if (*b >= '0' && *b <= '9')
		{
			int n=0;

			while (b != e && *b >= '0' && *b <= '9')
			{
				n = n * 10 + (*b-'0');
				++b;
			}

			if (b != e)
			{
				if (*b == ':')
					++b;   // Handle %H:%M:%S
				else if (*b == '-')
					++b;   // Handle the %d in %d-%b-%Y
			}

			if (plusminus)
				tz=((n % 100) + (n / 100) * 60)
					* (plusminus == '-' ? -60:60);
			else
				numbers.push_back(n);
			continue;
		}

		std::basic_stringstream<char_t> ss;

		while (b != e)
		{
			if (ct.is(*b, std::ctype_base::space))
				break;

			if (*b == '-')
			{
				// Handle the %b in %d-%b-%Y
				++b;
				break;
			}
			ss << *b;
			++b;
		}

		tm tmp=tm();

		tmp.tm_mon= -1;

		std::ios_base::iostate s;

		time_get->getFacetConstRef()
			.get_monthname(std::istreambuf_iterator<char_t>(ss),
				       std::istreambuf_iterator<char_t>(),
				       ss, s, &tmp);
		if (tmp.tm_mon >= 0 && tmp.tm_mon < 12)
		{
			month=tmp.tm_mon+1;
		}
	}

	if (numbers.size() != 5 || month == 0)
		ymdhms::cannot_parse_time();

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
#endif

#if 0
{
#endif
}
#endif
