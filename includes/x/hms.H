/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_hms_H
#define x_hms_H

#include <x/ymd.H>
#include <x/interval.H>
#include <x/strtok.H>
#include <x/exception.H>
#include <x/locale.H>
#include <x/ymdfwd.H>
#include <string>
#include <string_view>
#include <type_traits>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <list>
#include <vector>
#include <compare>
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
	~hms() {}

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

	explicit hms(//! The %interval
		     const std::string_view &intervalStr,

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

	std::string verboseString(const const_locale &l=
				  locale::base::environment())
		const;

	//! Convert to hh:mm:ss

	std::string hhmmss(//! Locale for the conversion
			   const const_locale
			   &localeRef=locale::base::global())
		const;

	//! Convert using a strftime format.

	std::string format_time(//! Time formatting pattern
				const std::string_view &pattern,
				//! Locale for the conversion
				const const_locale
				&localeRef=locale::base::global())
		const;

	//! Convert to "x hours, x minutes, x seconds"
	std::string toIntervalString(const const_locale &localeRef=locale::base::global())
		const;

	//! The stock to_string() uses toIntervalString()

	template<typename OutputIterator>
	OutputIterator to_string(//! Output iterator
				OutputIterator iter,

				//! Locale for the conversion
				const const_locale &localeRef=locale::base::global())
		const
	{
		std::string s=toIntervalString(localeRef);

		return std::copy(s.begin(), s.end(), iter);
	}

	//! Convert to a strftime-formatted string

	template<typename OutputIterator>
	OutputIterator to_string(//! Output iterator
				OutputIterator iter,

				//! Locale for the conversion
				const const_locale &localeRef,

				//! Pattern for the conversion
				const std::string_view &pattern)
		const;

	//! Create an interval from a string

	template<typename InputIterator>
	static hms from_string(//! Beginning iterator
			      InputIterator beg_iter,

			      //! Ending iterator
			      InputIterator end_iter,

			      //! The locale
			      const const_locale &localeArg=locale::base::global())

	{
		return hms(beg_iter, end_iter, localeArg);
	}

	//! Comparison operator

	std::strong_ordering operator<=>(const hms &o) const noexcept
	{
		hms a(*this);
		hms b(o);

		normalize(a.h, a.m, a.s, 0, 0);
		normalize(b.h, b.m, b.s, 0, 0);

		if (a.h < b.h)
			return std::strong_ordering::less;
		if (a.h > b.h)
			return std::strong_ordering::greater;
		if (a.m < b.m)
			return std::strong_ordering::less;
		if (a.m > b.m)
			return std::strong_ordering::greater;
		if (a.s < b.s)
			return std::strong_ordering::less;
		if (a.s > b.s)
			return std::strong_ordering::greater;

		return std::strong_ordering::equal;
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

private:
	//! Internal string array

	static const char * const interval_descr[];

	//! Report an error parsing the constructing string

	//! \internal
	//!
	static void parsing_error() __attribute__((noreturn));

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

	template<typename ptr_type, typename iter_type>
	static inline void serialize(ptr_type ptr, iter_type &i)
	{
		i(ptr->h);
		i(ptr->m);
		i(ptr->s);
	}
};

template<typename OutputIterator>
OutputIterator hms::to_string(OutputIterator iter,
			     const const_locale &localeRef,
			     const std::string_view &pattern) const
{
	std::string str(this->format_time(pattern, localeRef));

	return std::copy(str.begin(), str.end(), iter);
}

template<typename InputIterator>
void hms::construct(InputIterator beg_iter,
		    InputIterator end_iter,
		    const const_locale &l)
{
	typedef typename std::iterator_traits<InputIterator>
		::value_type char_type;

	std::basic_string<char_type> str(beg_iter, end_iter);

	if (str.find(':') != str.npos)
	{
		// Probably hh:mm:ss

		std::list<std::basic_string<char_type> > units;

		char_type delim[2];

		delim[0]=':';
		delim[1]=0;

		strtok_str(str, delim, units);

		if (units.size() > 0 && units.size() <= 3)
		{
			std::list<int32_t> units_n;

			while (!units.empty())
			{
				std::basic_string<char_type> &s=units.front();

				if (s.size() == 0)
					break;

				std::basic_istringstream<char_type> i(s);

				int32_t v;

				i >> v;

				if (i.fail())
					break;

				units_n.push_back(v);
				units.pop_front();
			}
			if (!units.empty())
				parsing_error();

			h=0;
			m=0;
			s=0;

			switch (units_n.size()) {
			case 1:
				s=units_n.front();
				break;
			case 2:
				h=units_n.front();
				units_n.pop_front();
				m=units_n.front();
				break;
			default:
				h=units_n.front();
				units_n.pop_front();
				m=units_n.front();
				units_n.pop_front();
				s=units_n.front();
				break;
			}

			normalize(h, m, s, 0, 0);
			return;
		}
	}
	else
	{
		interval<int32_t> parser(interval_descr, 2, libmsg());

		std::vector<int32_t> vec=parser.parse(str, l);

		normalize(vec[0], vec[1], vec[2], vec[3], vec[4]);
		h=vec[0];
		m=vec[1];
		s=vec[2];
		return;
	}

	parsing_error();
}

//! Overload the << operator.

std::ostream &operator<<(std::ostream &o, const hms &);

#ifndef DOXYGEN
extern template
void hms::construct(std::string::const_iterator,
		    std::string::const_iterator,
		    const const_locale &);
#endif

#if 0
{
#endif
}
#endif
