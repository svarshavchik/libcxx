/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ymdfwd_H
#define x_ymdfwd_H

#include <x/exception.H>
#include <x/namespace.h>
#include <x/localefwd.H>
#include <x/messagesfwd.H>
#include <x/facet.H>
#include <x/strftime.H>
#include <x/functional.H>
#include <stdint.h>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <type_traits>
#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A year, month, and a day

//! This class stores a date, from Jan 1 0001 to Dec 31 9999.
//! This class follows the Julian calendar until Sep 2 1752, then begins
//! with the Gregorian calendar on Sep 14 1752.

class ymd {

public:
	//! A day number. Jan 1 0001 is day 0.
	typedef uint32_t daynum_t;

	//! The signed day number value.
	typedef int32_t sdaynum_t;

private:
	//! Year
	uint16_t year;

	//! Month
	uint8_t month;

	//! Day of the month
	uint8_t day;

	//! A year whose January 1st falls on a Sunday

	static constexpr int jan1suyear=2006;

	//! Jan 1, 0001 is day 0.

	daynum_t daynum;

	//! Calculate the day number

	void compute_daynum()
	{
		daynum=compute_daynum(year, month, day);
	}

public:
	//! Compute the day number for the given date

	static daynum_t compute_daynum(//! Year
				       uint16_t year,

				       //! Month
				       uint8_t month,

				       //! Day of the month
				       uint8_t day);

	//! Retrieve the year
	uint16_t get_year() const noexcept
	{
		return year;
	}

	//! Retrieve the month
	uint16_t get_month() const noexcept
	{
		return month;
	}

	//! Retrieve the day of the month.

	uint16_t get_day() const noexcept
	{
		return day;
	}

	//! A date %interval

	//! This class represents some date %interval

	class interval {
	public:
		//! Number of years in the %interval

		sdaynum_t years;

		//! Number of months in the %interval
		sdaynum_t months;

		//! Number of weeks in the %interval
		sdaynum_t weeks;

		//! Number of days in the %interval
		sdaynum_t days;

		//! The default constructor

		//! All %interval values are zero
		interval() noexcept : years(0), months(0), weeks(0), days(0) {}

		//! Complete constructor

		interval(sdaynum_t yearsArg,
			 sdaynum_t monthsArg,
			 sdaynum_t weeksArg,
			 sdaynum_t daysArg) noexcept
			: years(yearsArg), months(monthsArg),
			  weeks(weeksArg), days(daysArg) {}

		//! Initialize the %interval to a given number of days

		//! This constructor is automatically used in date arithmetic
		//! methods of the ymd class.

		interval(//! Number of days
			 sdaynum_t daysArg) noexcept
			: years(0), months(0), weeks(0), days(daysArg)
		{
		}

		//! Initialize the %interval from a text string.

		interval(//! Text string
			 const std::string &Arg,

			 //! The locale
			 const const_locale &localeArg=locale::base::global()
			 );

		//! Negate the %interval

		interval operator-() const;

		//! Add two %intervals together

		interval &operator+=(const interval &o);

		//! Add two %intervals together

		interval operator+(const interval &o) const
		{
			interval n(*this);

			n += *this;
			return n;
		}

		//! Convert this %interval to a descriptive string

		std::string internal_tostring
		(//! Use this locale for the description
		 const const_locale &l=
		 locale::base::global()) const;

		//! Convert this %interval to a descriptive string

		template<typename OutputIterator>
		OutputIterator toString(//! Output iterator
					OutputIterator iter,
					//! Locale for the conversion
					const const_locale &localeRef=locale::base::global())
			const;

		//! Convert this %interval to a descriptive string

		//! Returns a string in the default locale

		operator std::string() const;

		//! Create an interval from a string

		template<typename InputIterator>
		static interval fromString(//! Beginning iterator
					   InputIterator beg_iter,

					   //! Ending iterator
					   InputIterator end_iter,

					   //! The locale
					   const const_locale &localeArg
					   =locale::base::global())
		{
			return interval(std::string(beg_iter, end_iter),
					localeArg);
		}

		//! Serialization function

		template<typename iter_type>
		void serialize(iter_type &i)
		{
			i(years);
			i(months);
			i(weeks);
			i(days);
		}

	private:
		//! Internal string array

		static const char * const interval_descr[];
	};

	//! A convenience function for creating an %interval of X years

	//! \code
	//! ymd dateValue;
	//!
	//! // ...
	//!
	//! dateValue += ymd::years(1);
	//! \endcode

	static interval years(//! Number of years in the %interval
			      sdaynum_t n) noexcept
	{
		interval r;

		r.years=n;
		return r;
	}

	//! A convenience function for creating an %interval of X months

	//! \code
	//! ymd dateValue;
	//!
	//! // ...
	//!
	//! dateValue += ymd::months(1);
	//! \endcode

	static interval months(//! Number of months in the %interval
			       sdaynum_t n) noexcept
	{
		interval r;

		r.months=n;
		return r;
	}

	//! A convenience function for creating an %interval of X weeks

	//! \code
	//! ymd dateValue;
	//!
	//! // ...
	//!
	//! dateValue += ymd::weeks(1);
	//! \endcode

	static interval weeks(//! Number of weeks in the %interval
			      sdaynum_t n) noexcept
	{
		interval r;

		r.weeks=n;
		return r;
	}

	//! Gregorian reformation's year

	static constexpr uint16_t gregorian_reformation_date_year=1752;

	//! Gregorian reformation's month

	static constexpr uint8_t gregorian_reformation_date_month=9;

	//! Gregorian reformation's day

	static constexpr uint8_t gregorian_reformation_date_day=2;

	//! Number of days skipped in the Gregorian reformation

	static constexpr int gregorian_reformation_days=11;

	//! The day number for Gregorian reformation

	static const daynum_t gregorian_reformation_date_jd;

	//! Initialize the date to today
	ymd();

	//! The destructor
	~ymd();

	//! Initialize the date to a given date

	ymd(uint16_t year, uint8_t month, uint8_t day);

	//! Return the largest date

	static ymd max();

	//! Compute the date's day of the week.

	//! \return 0 for Sunday.

	int get_day_of_week() const noexcept
	{
		return (jd()+6) % 7;
	}

	//! Return the date's day number.

	//! Jan 1, 0001 is day 0.
	//!
	daynum_t jd() const noexcept { return daynum; }

	//! Initialize date to the day number.

	//! Jan 1, 0001 is day 0.
	//!

	explicit ymd(daynum_t dayNumber);

	//! Add the given number of days.

	ymd &operator+=(//! How many days to add
			const interval &days);

	//! Subtract the given number of days.

	ymd &operator-=(//! Number of days to add
			const interval &days)
	{
		return operator+=(-days);
	}

	//! Increment the date

	ymd &operator++()
	{
		return operator +=(1);
	}

	//! Increment the date

	ymd operator++(int)
	{
		ymd c(*this);

		++*this;

		return c;
	}

	//! Decrement the date

	ymd &operator--()
	{
		return operator -=(1);
	}

	//! Decrement the date

	ymd operator--(int)
	{
		ymd c(*this);

		--*this;

		return c;
	}

	//! Add the given number of days.

	ymd operator+(const interval &dayCount) const
	{
		ymd d= *this;

		d += dayCount;

		return d;
	}

	//! Subtract the given number of days.

	ymd operator-(const interval &dayCount) const
	{
		ymd d= *this;

		d -= dayCount;

		return d;
	}

	//! Calculate number of days between two dates

	sdaynum_t operator-(//! The other date
			    const ymd &other) const noexcept;

	//! Compare this date with another one.

	bool operator==(const ymd &other) const noexcept
	{
		return year == other.year &&
			month == other.month &&
			day == other.day;
	}

	//! Compare this date with another one.

	bool operator!=(const ymd &other) const noexcept
	{
		return !operator==(other);
	}

	//! Compare this date with another one.

	bool operator<(const ymd &other) const noexcept
	{
		return year < other.year ? true:
			year > other.year ? false:
			month < other.month ? true:
			month > other.month ? false:
			day < other.day;
	}

	//! Compare this date with another one.

	bool operator>(const ymd &other) const noexcept
	{
		return year > other.year ? true:
			year < other.year ? false:
			month > other.month ? true:
			month < other.month ? false:
			day > other.day;
	}

	//! Compare this date with another one.

	bool operator<=(const ymd &other) const noexcept
	{
		return !operator>(other);
	}

	//! Compare this date with another one.

	bool operator>=(const ymd &other) const noexcept
	{
		return !operator<(other);
	}

	//! Is this year a leap year

	static inline bool leapyear(uint16_t y) noexcept
	{
		return (y < gregorian_reformation_date_year ?
			(y % 4) == 0:(y % 400) == 0 ||
			((y % 100) && (y % 4) == 0));
	}


	//! First day of week of the given year.

	static ymd firstDayOfWeek(//! The given year
				  uint16_t year,

				  //! Day of week.

				  //! If 0, returns the date of the first
				  //! Sunday in the given year, if 1, the first
				  //! Monday, etc..
				  int weekday)
	{
		ymd d(year, 1, 1);

		d += (weekday + 7 - d.get_day_of_week()) % 7;

		return d;
	}

	//! An ISO8601 date

	class iso8601 {

		//! The ISO8601 year
		uint16_t year;

		//! The ISO8601 week
		uint8_t week;

		//! The ISO8601 day of week (1-7). Monday is day 1.
		uint8_t daynum;

	public:
		//! Create a new ISO8601 date

		iso8601(const ymd &date);

		//! Create a new ISO8601 date

		iso8601(//! The ISO8601 year
			uint16_t yearArg,

			//! The ISO8601 week
			int weekArg,

			//! The ISO8601 day of week (1-7). Monday is day 1.
			int dayArg);

		//! Default destructor
		~iso8601();

		//! Return the ISO8601 year

		inline uint16_t get_year() const noexcept { return year; }

		//! Return the ISO8601 week

		inline uint16_t get_week() const noexcept { return week; }

		//! Return the ISO8601 day of week. Monday is day 1.

		inline uint16_t get_day_of_week() const noexcept { return daynum; }

		//! Convert this object to a text string

		std::string toString() const;

		//! String operator

		inline operator std::string() const
		{
			return toString();
		}

		//! Serialization function

		template<typename iter_type>
		inline void serialize(iter_type &i)
		{
			i(year);
			i(week);
			i(daynum);
		}
	};

	//! Construct from an ISO8601 date

	ymd(const iso8601 &iso8601Date);

	//! Convert the date to a text string.

	//! The date is formatted according to the given \c strftime(3)
	//! pattern. Only date-related formatting conversion in the formatting
	//! string carry meaningful results.

	template<typename OutputIterator>
	OutputIterator toString(//! Output iterator
				OutputIterator iter,
				//! Locale for the conversion
				const const_locale &localeRef=locale::base::global(),
				//! Start of the pattern
				const char *pattern=0)
		const;

	//! Convert date to a text string.

	//! \overload
	//!
	template<typename OutputIterator>
	OutputIterator toString(//! Output iterator
				OutputIterator iter,

				//! Locale for the conversion
				const const_locale &localeRef,

				//! Formatting pattern
				const std::string &pattern)
		const;

	//! Convert date to a text string.

	//! \overload
	//!
	std::string format_date(//! Formatting pattern
				const char *pattern=0,
				//! Locale for the conversion
				const const_locale
				&localeRef=locale::base::global())
		const;

	//! Convert date to a text string.

	//! \overload
	//!
	std::string format_date(//! Pattern for the conversion
				const std::string &pattern,
				//! Locale for the conversion
				const const_locale
				&localeRef=locale::base::global())
		const;

	//! Convert date to a text string.

	operator std::string() const;

	//! A helper class for parsing an ymd value from a string

	class parser {

		//! Locale used for parsing
		const_locale loc;

		//! Cached month names

		std::u32string month_names[12];

		//! Cached weekday names
		std::u32string month_names_long[12];

		//! Flag: interpret dd/dd/dddd as m/d/y

		bool usmdy;

	public:
		//! Constructor
		parser(const const_locale &loc=locale::base::environment());

		//! Destructor
		~parser();

		//! Set flag to interpret dd/dd/dddd as mm/dd/yyyy.

		inline parser &mdy(bool flag) noexcept
		{
			usmdy=flag;
			return *this;
		}

		//! Make some sense out of a string

		//! This parser accepts a date specified in a variety of formats
		//! including: YYYY-MM-DD, DD month YYYY, month DD YYYY and
		//! AA/BB/YYYY (with the mdy() flag specifying whether AA and BB
		//! is parsed as a month or a day).
		//!
		//! \return an mdy object, or throws an %exception if the
		//! string cannot be parsed.

		template<typename InputIterator>
		ymd parse(InputIterator beg_iter,
			  InputIterator end_iter);

		//! Internal parser

		//! \internal
		ymd parse_internal(const std::u32string &);

		//! Make some sense out of a string

		//! \overload

		ymd parse(//! The string
			  const char *string);

		//! Make some sense out of a string

		//! \overload
		inline ymd parse(//! The string
				 const std::string &s)

		{
			return parse(s.c_str());
		}
	};

	//! Initialize the date from a string

	ymd(const char *string,
	    bool mdyflag=false,
	    const const_locale &loc=locale::base::global());

	//! Initialize the date from a string

	ymd(const std::string &string,
	    bool mdyflag=false,
	    const const_locale &loc=locale::base::global());

	//! Instantiate a new object from an input iterator

	//! A string gets retrieved from the given beginning and ending
	//! iterator. The string may be in any format that's recognized
	//! by the \ref parser "parser object".

	template<typename InputIterator>
	static inline ymd fromString(//! Beginning iterator
				     InputIterator beg_iter,

				     //! Ending iterator
				     InputIterator end_iter,

				     //! The locale
				     const const_locale &localeArg
				     =locale::base::global())
	{
		return parser(localeArg).parse(beg_iter, end_iter);
	}

	//! Serialization function

	template<typename iter_type>
	inline void serialize(iter_type &i)
	{
		i(year);
		i(month);
		i(day);
	}
};

#if 0
{
#endif
}
#endif
