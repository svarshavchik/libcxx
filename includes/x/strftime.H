/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_strftime_H
#define x_strftime_H

#include <x/exception.H>
#include <x/localeobj.H>
#include <x/tzfile.H>
#include <x/facet.H>
#include <x/ptr.H>
#include <time.h>

#include <string>

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class ymdhms;
class ymd;
class hms;

//! Common locale-related functions used by \ref strftime "strftime".

class strftime_base : public notcopyableObj {

protected:

	//! Locale for the time format
	const_locale l;

	//! IOS formatting flags with the imbued locale
	std::ios ios;

	//! Shortcut for the locale facet reference.
	typedef ref<facetObj< std::time_put<char> > >
	strftime_ref_t;

	//! Reference to the std::time_put locale.
	strftime_ref_t facetRef;

	//! Shortcut for the locale facet nullable pointer.
	typedef ptr<facetObj< std::time_put<char> > >
	strftime_ptr_t;

	//! Nullable pointer to the std::time_put locale.
	strftime_ptr_t facetPtr;

	//! Direct reference to the native C++ library facet object.

	const std::time_put<char> &facetRefImpl;

	//! Initialize facets

	strftime_base(const const_locale &lArg);

	//! Destructor
	~strftime_base() noexcept;
};

//! Date and time formatting

//! \code
//! locale l(locale::create("en_US.utf8"));
//!
//! std::cout << "Current time is " << strftime()("%Y-%m-%d %H:%M:%S") << std::endl;
//!
//! tzfile nyc(tzfile::create("America/New_York"));
//!
//! std::cout << "Happy New Year, New York: "
//!           << strftime(ymdhms(ymd(2000, 1, 1), hms(0, 0, 0), nyc))
//!                       ("%Y-%m-%d %H:%M:%S") << std::endl;
//! \endcode
//!
//! This objects formats a date and/or time of day using a \c strftime(3) format
//! string. The date/time may be specified in a number of ways:
//!
//! - a \c time_t calendar time. A timezone specification converts the
//! \c time_t value to a date and time. The calendar time defaults to the
//! current time, and the timezone defaults to the system default timezone.
//!
//! - a \c tm structure. This is the traditional parameter to
//! strftime(3)
//!
//! - a ymdhms object that defines the date and the time of day.
//!
//! - a ymd object that defines the date component only. Only date-related
//! conversion specifications in the \c strftime(3)
//! format string produce meaningful results,
//! the time of day component is fixed to midnight on the given date.
//!
//! - a hms object that defines the time of day component only. Only time
//! of day-related conversion specifications in the \c strftime(3) conversion
//! string produce meaningful results.
//!
//! The date/time specification is passed to the constructor, together with
//! the timezone specification (if applicable), and the governing locale for
//! the format string (this defaults to the current global locale).
//!
//! The format string gets passed to the constructed object using the
//! \c () operator, which returns the formatted date and time.
//! Another format string can be passed again, resulting in another
//! formatted date and time, that references the saved date/time value.
//! The constructed object may be recycled: another date/time specification
//! may be passed to the \c () operator, which replaces the saved date and time
//! for subsequent format strings.

class strftime : public strftime_base {

	//! The time to be formatted.
	struct tm tm;

	//! Buffer for the timezone pointer.
	std::string tm_tz;

	//! Governing timezone definition
	const_tzfile tz;

public:

	//! Default constructor

	//! The constructor set the time conversion locale, and initializes
	//! the time value to the current time.

	strftime(//! Timezone for conversion
		 const const_tzfile &tzArg=tzfile::base::local(),

		 //! Time conversion locale
		 const const_locale &lArg=locale::base::global());

	//! Constructor

	//! \overload
	//!
	strftime(//! Date and time to format
		 const ymdhms &datetime,

		 //! Effective locale

		 const const_locale &lArg=locale::base::global());

	//! Constructor

	//! \overload
	//!
	strftime(//! Date and time to format
		 const ymd &dateValue,

		 //! Effective locale
		 const const_locale &lArg=locale::base::global(),

		 //! Effective timezone
		 const const_tzfile &tzArg=tzfile::base::local());

	//! Constructor

	//! \overload
	//!
	strftime(//! Date and time to format
		 const hms &time,

		 //! Effective locale

		 const const_locale &lArg=locale::base::global());

	//! Constructor

	//! \overload
	//!
	strftime(//! Use this time value for formatting.
		 time_t t,
		 //! Use the following timezone
		 const const_tzfile &tzArg=tzfile::base::local(),
		 //! Time conversion locale
		 const const_locale &lArg=locale::base::global());

	//! Constructor

	//! \overload
	//!
	strftime(//! Use this time value for formatting.
		 const struct tm *tmArg,

		 //! Time conversion locale
		 const const_locale &lArg=locale::base::global());

	//! Destructor
	~strftime() noexcept;

	//! Specify what date/time is to be formatted

	//! \return a reference to this object.
	//!
	strftime &operator()(//! Format this time
			     const struct tm *tm_cpy) noexcept;

	//! Specify what date/time is to be formatted

	//! \overload
	//!

	strftime &operator()(//! Format this time
			     time_t timeVal);

	//! Specify what date/time is to be formatted

	//! \overload
	//!

	strftime &operator()(//! Format this time
			     time_t timeVal,

			     //! Effective timezone

			     const const_tzfile &tzArg);

	//! Specify what date/time is to be formatted

	//! \overload
	//!

	strftime &operator()(//! Format this time
			     const ymdhms &datetime);

	//! Specify what date/time is to be formatted

	//! \overload
	//!

	strftime &operator()(//! Format this time
			     const ymd &date);

	//! Specify what date/time is to be formatted

	//! \overload
	//!

	strftime &operator()(//! Format this time
			     const ymd &date,

			     //! Timezone for conversion
			     const const_tzfile &tzArg);

	//! Specify what date/time is to be formatted

	//! \overload
	//!

	strftime &operator()(//! Format this time
			     const hms &time);

	//! Format the specified time

	//! \return the formatted string for the previously specified time
	//! value, as a string.
	std::string operator()(//! Time format string

			       //! See \c strftime(3) for more
			       //! information on the time format
			       //! template string.
			       const std::string &str);

	//! Format the specified time

	//! \overload
	//!

	std::string operator()(//! Time format string
			       const char *str);

	//! Format the specified time

	//! \overload
	//! Rather than returning the formatted time, as a string, it
	//! gets written to an output stream.

	strftime &operator()(//! Time format string
			     const std::string &str,

			     //! Formatted time gets written to this stream
			     std::ostream &o);

	//! Format the specified time

	//! \overload
	//!
	strftime &operator()(//! Time format string
			     const char *str,
			     //! Formatted time gets written to this stream
			     std::ostream &o);
};

#if 0
{
#endif
}
#endif
