/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_hmsfwd_H
#define x_hmsfwd_H

#include <x/exception.H>
#include <x/namespace.h>
#include <x/locale.H>
#include <x/exception.H>
#include <x/ymdfwd.H>
#include <x/value_stringable.H>
#include <string>
#include <algorithm>
#include <type_traits>
#include <stdint.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Hours, minutes, and seconds

//! This class serves two roles:
//!
//! - Specify an %interval of time specified as hours, minutes, and
//! seconds.
//!
//! - Store the time portion of an ymdhms object.
//!
//! A class instance that's constructed from a string may take a string
//! containing either "HH:MM:SS", or something more verbose, such as
//! "10 hours, 1 minute".

class hms {

public:

	//! Hours value
	int32_t h;

	//! Minutes value
	int32_t m;

	//! Seconds value
	int32_t s;

	//! Default constructor

	hms() noexcept : h(0), m(0), s(0) {}

	//! Default destructor
	~hms() noexcept {}

	//! Construct to an explicit time %interval

	hms(//! Seconds
	    time_t seconds);

	//! Construct to an explicit time %interval

	hms(//! Hours
	    int32_t hv,

	    //! Minutes
	    int32_t mv,

	    //! Seconds
	    int32_t sv) : h(hv), m(mv), s(sv)
	{
	}

	//! Construct from days or weeks
	hms(//! The date %interval must only specify days and weeks.
	    //! An exception gets thrown if the %interval contains months or
	    //! years.
	    const ymd::interval &ymd_interval);


	//! Construct from a descriptive string

	//! The time %interval can be specified by a freeform string, such as
	//! "10 hours, 5 minutes, 1 second". This is a very loose parser.
	//! individual words may be separated by whitespace or punctuation.
	//! A numeric value can either precede or succeed its word, so
	//! "hour 10, minutes 5, second 1" also specifies a time %interval
	//! of 10:05:01. The parser understands the words for seconds, minutes,
	//! hours, days, and weeks. One day is just an alias for 24 hours,
	//! and a week is an alias for 24*7 hours.
	//!
	//! This constructor will also process the strings "HH:MM:SS" and
	//! "HH:MM". A string consisting of a single number gets processed
	//! as a number of seconds.

	hms(//! The %interval
	    const std::string &intervalStr,

	    //! The locale of the given string
	    const const_locale &l=locale::base::global());

	//! Construct from a descriptive string

	//! \overload
	//!
	hms(//! The %interval
	    const std::wstring &intervalStr,

	    //! The locale of the given string
	    const const_locale &l=locale::base::global());

	//! Construct from a descriptive string

	hms(const char *intervalStr,
	    const const_locale &l=locale::base::global());

	//! Construct from a descriptive string

	hms(//! The %interval
	    const wchar_t *intervalStr,

	    //! The locale of the given string
	    const const_locale &l=locale::base::global());

	//! Construct from a beginning and ending iterator

	//! The string is specified using iterators.
	//!
	template<typename InputIterator>
	hms(//! Beginning iterator
	    InputIterator beg_iter,

	    //! Ending iterator
	    InputIterator end_iter,

	    //! The locale of the given string
	    const const_locale &l=locale::base::global())
	{
		construct(beg_iter, end_iter, l);
	}

	//! Negate this %interval

	hms operator-() const;

	//! Add two %intervals together

	//! The result is always a normalized time interval: the minutes
	//! and seconds are always in the range of 00-59 (a negative time
	//! interval will have the minutes and seconds value in the range 00
	//! through -59).

	hms &operator+=(const hms &o);

	//! Subtract two %intervals

	//! The result is always a normalized time interval: the minutes
	//! and seconds are always in the range of 00-59 (a negative time
	//! interval will have the minutes and seconds value in the range 00
	//! through -59).

	hms &operator-=(const hms &o)
	{
		return operator+=(-o);
	}

	//! Add two %intervals together

	//! The result is always a normalized time interval: the minutes
	//! and seconds are always in the range of 00-59 (a negative time
	//! interval will have the minutes and seconds value in the range 00
	//! through -59).

	hms operator+(const hms &o) const
	{
		hms cpy(*this);

		cpy += o;

		return cpy;
	}

	//! Subtract two time intervals

	//! The result is always a normalized time interval: the minutes
	//! and seconds are always in the range of 00-59 (a negative time
	//! interval will have the minutes and seconds value in the range 00
	//! through -59).

	hms operator-(const hms &o) const
	{
		hms cpy(*this);

		cpy += -o;

		return cpy;
	}

	//! Convert the time value to a number of seconds

	time_t seconds() const;

	//! Convert this %interval to a descriptive string

	template<typename CharT>
	std::basic_string<CharT> verboseString(const const_locale &l=
					       locale::base::environment())
		const;

	//! Convert to hh:mm:ss

	template<typename char_type>
	std::basic_string<char_type> hhmmss(//! Pattern for the conversion
					    const char_type *pattern=0,
					    //! Locale for the conversion
					    const const_locale
					    &localeRef=locale::base::global())
		const;

	//! Convert to hh:mm:ss

	template<typename char_type>
	std::basic_string<char_type> hhmmss(//! Pattern for the conversion
					    const std::basic_string<char_type>
					    &pattern,
					    //! Locale for the conversion
					    const const_locale
					    &localeRef=locale::base::global())
		const;

	//! Convert to "x hours, x minutes, x seconds"
	std::string toIntervalString(const const_locale &localeRef=locale::base::global())
		const;

	//! This class implements toString() and fromString()
	static const stringable_t stringable=class_tostring;

	//! The stock toString() uses toIntervalString()

	template<typename OutputIterator>
	OutputIterator toString(//! Output iterator
				OutputIterator iter,

				//! Locale for the conversion
				const const_locale &localeRef=locale::base::global())
		const
	{
		std::string s=toIntervalString(localeRef);

		return std::copy(s.begin(), s.end(), iter);
	}

	//! Convert to a strftime-formatted string

	template<typename OutputIterator, typename char_type>
	OutputIterator toString(//! Output iterator
				OutputIterator iter,

				//! Locale for the conversion
				const const_locale &localeRef,

				//! Pattern for the conversion
				const char_type *pattern)
		const;

	//! Convert to a strftime-formatted string

	template<typename OutputIterator, typename char_type>
	OutputIterator toString(//! Output iterator
				OutputIterator iter,

				//! Locale for the conversion
				const const_locale &localeRef,

				//! Pattern for the conversion
				const std::basic_string<char_type> &pattern)
		const;

	//! Create an interval from a string

	template<typename InputIterator>
	static hms fromString(//! Beginning iterator
			      InputIterator beg_iter,

			      //! Ending iterator
			      InputIterator end_iter,

			      //! The locale
			      const const_locale &localeArg=locale::base::global())

	{
		return hms(beg_iter, end_iter, localeArg);
	}

	//! Comparison operator

	bool operator<(const hms &o) const noexcept
	{
		hms a(*this);
		hms b(o);

		normalize(a.h, a.m, a.s, 0, 0);
		normalize(b.h, b.m, b.s, 0, 0);

		return a.h < b.h || (a.h == b.h
				     && a.m * 60 + a.s < b.m * 60 + b.s);
	}

	//! Comparison operator

	bool operator>(const hms &o) const noexcept
	{
		hms a(*this);
		hms b(o);

		normalize(a.h, a.m, a.s, 0, 0);
		normalize(b.h, b.m, b.s, 0, 0);

		return a.h > b.h || (a.h == b.h
				     && a.m * 60 + a.s > b.m * 60 + b.s);
	}

	//! Comparison operator

	bool operator==(const hms &o) const noexcept
	{
		hms a(*this);
		hms b(o);

		normalize(a.h, a.m, a.s, 0, 0);
		normalize(b.h, b.m, b.s, 0, 0);

		return (a.h == b.h && a.m == b.m && a.s == b.s);
	}

	//! Comparison operator

	bool operator!=(const hms &o) const noexcept
	{
		return !operator==(o);
	}

	//! Comparison operator

	bool operator>=(const hms &o) const noexcept
	{
		return !operator<(o);
	}

	//! Comparison operator

	bool operator<=(const hms &o) const noexcept
	{
		return !operator>(o);
	}

private:
	//! Internal string array

	static const char * const interval_descr[];

	//! Report an error parsing the constructing string

	//! \internal
	//!
	static void parsing_error()
 __attribute__((noreturn));

	//! Set the current value

	//! The minute/second values are normalized to the range -59..59
	//!

	template<typename int_type>
	static void normalize(//! Hours
			      int_type &h,

			      //! Minutes
			      int_type &m,

			      //! Seconds
			      int_type &s,

			      //! Days
			      int_type d,

			      //! Weeks
			      int_type w);

	//! Internal initialization function
	template<typename InputIterator>
	void construct(InputIterator beg_iter,
		       InputIterator end_iter,
		       const const_locale &l);

public:
	//! Serialization function
		
	template<typename iter_type>
	void serialize(iter_type &i)
	{
		i(h);
		i(m);
		i(s);
	}
};

#if 0
{
#endif
}
#endif
