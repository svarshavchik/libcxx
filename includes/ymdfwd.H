/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ymdfwd_H
#define x_ymdfwd_H

#include <x/exception.H>
#include <x/namespace.h>
#include <x/locale.H>
#include <x/messagesfwd.H>
#include <x/facet.H>
#include <x/ctype.H>
#include <x/strftimefwd.H>
#include <x/value_stringable.H>
#include <stdint.h>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <type_traits>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A year, month, and a day

//! This class stores a date, from Jan 1 0001 to Dec 31 9999.
//! This class follows the Julian calendar until Sep 2 1752, then begins
//! with the Gregorian calendar on Sep 14 1752.

class ymd {

	//! Throw an %exception when there's a failure parsing the date

	//! \overload

	static void date_parsing_failed() __attribute((noreturn));

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

	static const int jan1suyear=2006;

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
	uint16_t getYear() const noexcept
	{
		return year;
	}

	//! Retrieve the month
	uint16_t getMonth() const noexcept
	{
		return month;
	}

	//! Retrieve the day of week. Sunday is day 0.

	uint16_t getDay() const noexcept
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

		//! Initialize the %interval from a text string

		interval(//! The %interval
			 const std::string &intervalStr,

			 //! The locale
			 const const_locale &localeArg=locale::base::global()
			 );

		//! Initialize the %interval from a text string defined by a beginning and ending iterator

		template<typename InputIterator>
		interval(//! Beginning iterator
			 InputIterator beg_iter,

			 //! Ending iterator
			 InputIterator end_iter,

			 //! The locale
			 const const_locale &localeArg=locale::base::global()
			 );

	private:
		//! An internal constructor

		//! \internal

		template<typename InputIterator>
		void construct(InputIterator beg_iter,
			       InputIterator end_iter,
			       const const_locale &localeArg)
;
	public:

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

		template<typename char_type> std::basic_string<char_type>
		toString(//! Use this locale for the description
			 const const_locale &l=locale::base::global()) const;

		//! This class implements toString() and fromString()
		static const stringable_t stringable=class_tostring;

		//! Convert this %interval to a descriptive string

		template<typename OutputIterator>
		OutputIterator toString(//! Output iterator
					OutputIterator iter,
					//! Locale for the conversion
					const const_locale &localeRef=locale::base::global())
			const;

		//! Convert this %interval to a descriptive string

		//! Returns a string in the default locale

		template<typename char_type>
		operator std::basic_string<char_type>()
			const
		{
			return toString<char_type>();
		}

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
			return interval(beg_iter, end_iter, localeArg);
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

	static const uint16_t gregorian_reformation_date_year=1752;

	//! Gregorian reformation's month

	static const uint8_t gregorian_reformation_date_month=9;

	//! Gregorian reformation's day

	static const uint8_t gregorian_reformation_date_day=2;

	//! Number of days skipped in the Gregorian reformation

	static const int gregorian_reformation_days=11;

	//! The day number for Gregorian reformation

	static const daynum_t gregorian_reformation_date_jd;

	//! Initialize the date to today
	ymd();

	//! The destructor
	~ymd() noexcept;

	//! Initialize the date to a given date

	ymd(uint16_t year, uint8_t month, uint8_t day);

	//! Return the largest date

	static ymd max();

	//! Compute the date's day of the week.

	//! \return 0 for Sunday.

	int getDayOfWeek() const noexcept
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

		d += (weekday + 7 - d.getDayOfWeek()) % 7;

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
		~iso8601() noexcept;

		//! Return the ISO8601 year

		uint16_t getYear() const noexcept { return year; }

		//! Return the ISO8601 week

		uint16_t getWeek() const noexcept { return week; }

		//! Return the ISO8601 day of week. Monday is day 1.

		uint16_t getDayOfWeek() const noexcept { return daynum; }

		//! Convert this object to a text string

		template<typename CharT>
		std::basic_string<CharT> toString() const;

		//! String operator

		template<typename CharT>
		operator std::basic_string<CharT>() const
		{
			return toString<CharT>();
		}

		//! Serialization function
		
		template<typename iter_type>
		void serialize(iter_type &i)
		{
			i(year);
			i(week);
			i(daynum);
		}

	};

	//! Construct from an ISO8601 date

	ymd(const iso8601 &iso8601Date);

	//! This class implements toString(), fromString(), toWideString() and fromWideString().

	static const stringable_t stringable=class_toboth;

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

	//! Convert the date to a text string.

	//! The date is formatted according to the given \c strftime(3)
	//! pattern. Only date-related formatting conversion in the formatting
	//! string carry meaningful results.

	template<typename OutputIterator>
	OutputIterator toWideString(//! Output iterator
				    OutputIterator iter,

				    //! Locale for the conversion
				    const const_locale &localeRef=locale::base::global(),
				    //! Start of the pattern
				    const wchar_t *pattern=0)
		const;

	//! Convert date to a text string.

	//! \overload
	//!
	template<typename OutputIterator>
	OutputIterator toWideString(//! Output iterator
				    OutputIterator iter,

				    //! Locale for the conversion
				    const const_locale &localeRef,

				    //! Formatting pattern
				    const std::wstring &pattern)
		const;

	//! Convert date to a text string.

	//! \overload
	//!
	template<typename CharT>
	std::basic_string<CharT> formatDate(//! Formatting pattern
					    const CharT *pattern=0,
					    //! Locale for the conversion
					    const const_locale
					    &localeRef=locale::base::global())
		const;

	//! Convert date to a text string.

	//! \overload
	//!
	template<typename CharT>
	std::basic_string<CharT> formatDate(//! Pattern for the conversion
					    const std::basic_string<CharT>
					    &pattern,
					    //! Locale for the conversion
					    const const_locale
					    &localeRef=locale::base::global())
		const;

	//! Convert date to a text string.

	template<typename CharT>
	operator std::basic_string<CharT>() const;

	//! A helper class for parsing an ymd value from a string

	template<typename CharT> class parser {

		//! Locale used for parsing
		const_locale loc;

		//! Cached month names

		std::basic_string<CharT> month_names[12];

		//! Cached weekday names
		std::basic_string<CharT> month_names_long[12];

		//! Flag: interpret dd/dd/dddd as m/d/y

		bool usmdy;

	public:
		//! Constructor
		parser(const const_locale &loc=locale::base::environment());

		//! Destructor
		~parser() noexcept;

		//! Set flag to interpret dd/dd/dddd as mm/dd/yyyy.

		//! The default is dd/mm/yyyy
		//!

		parser<CharT> &mdy(bool flag=true) noexcept
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

		//! Make some sense out of a string

		//! \overload

		ymd parse(//! The string
			  const CharT *string);

		//! Make some sense out of a string

		//! \overload
		template<typename traits_type, typename alloc_type>
		ymd parse(//! The string
			  const std::basic_string<CharT, traits_type,
			  alloc_type> &s)

		{
			return parse(s.c_str());
		}
	};

	//! Initialize the date from a string

	ymd(const char *string,
	    bool mdyflag=false,
	    const const_locale &loc=locale::base::global())

	{
		*this= parser<char>(loc).mdy(mdyflag).parse(string);
	}

	//! Initialize the date from a string

	ymd(const wchar_t *string,
	    bool mdyflag=false,
	    const const_locale &loc=locale::base::global())

	{
		*this= parser<wchar_t>(loc).mdy(mdyflag).parse(string);
	}

	//! Initialize the date from a string

	template<typename CharT,
		 typename traits_type,
		 typename alloc_type> ymd(const std::basic_string<CharT,
					  traits_type, alloc_type> &string,
					  bool mdyflag=false,
					  const const_locale &loc=locale::base::global())

	{
		*this= parser<CharT>(loc).mdy(mdyflag).parse(string);
	}

	//! Instantiate a new object from an input iterator

	//! A string gets retrieved from the given beginning and ending
	//! iterator. The string may be in any format that's recognized
	//! by the \ref parser "parser object".

	template<typename InputIterator>
	static ymd fromString(//! Beginning iterator
			      InputIterator beg_iter,

			      //! Ending iterator
			      InputIterator end_iter,

			      //! The locale
			      const const_locale &localeArg
			      =locale::base::global())
	{
		return parser<char>(localeArg).parse(beg_iter, end_iter);
	}

	//! Instantiate a new object from an input iterator

	//! A string gets retrieved from the given beginning and ending
	//! iterator. The string may be in any format that's recognized
	//! by the \ref parser "parser object".

	template<typename InputIterator>
	static ymd fromWideString(//! Beginning iterator
				  InputIterator beg_iter,

				  //! Ending iterator
				  InputIterator end_iter,

				  //! The locale
				  const const_locale &localeArg
				  =locale::base::global())
	{
		return parser<wchar_t>(localeArg).parse(beg_iter, end_iter);
	}

	//! Serialization function
		
	template<typename iter_type>
	void serialize(iter_type &i)
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

