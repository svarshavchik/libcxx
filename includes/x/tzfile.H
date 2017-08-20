/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tzfile_H
#define x_tzfile_H

#include <string>
#include <iostream>
#include <sstream>

#include <stdint.h>

#include <x/fdfwd.H>
#include <x/vectorfwd.H>
#include <x/locale.H>
#include <x/singletonfwd.H>
#include <x/basicstreamobj.H>

#include <set>

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class ymd;
class hms;
class ymdhmsBase;

class tzfileObj;
class tzfileBase;

//! Timezone information

//! This is a reference to a reference-counted object that stores timezone
//! data. These timezone objects are separate from the global C environment
//! used by \c tzset(3) and \c ctime(3) family of functions. The C timezone
//! support uses global, static data. This is a separate implementation that
//! reads the same timezone database from \c /usr/share/zoneinfo.
//!
//! As such, this implementation does not support some obscure features of
//! \c tzset(3). Notably, this implementation does not support direct POSIX
//! timezone string specification in the \c TZ environment variable.
//! \c TZ, if set, must specify a valid timezone name that exists in the
//! system timezone database.
//!
//! These objects don't offer much beyond loading the timezone specification
//! from the system timezone data. These objects are used in conjunction with
//! ymdhms objects.
//!
//! \par Loading timezone data
//!
//! \code
//! tzfile nyc(tzfile::create("America/New_York"));
//! \endcode
//!
//! \ref tzfileObj::tzfileObj(const std::string &) "tzfile::create()"
//! loads timezone data
//! from the specified file in the timezone database. Multiple timezone objects
//! may exist, at the same time, holding different timezone data. This is
//! notably different from the global C environment timezone data. Given its
//! global nature, only one timezone file can be loaded into the C environment
//! at one time.
//!
//! \par Loading the UTC timezone
//!
//! \code
//! tzfile utc(tzfile::base::utc());
//! \endcode
//!
//! \link tzfileBase::utc() tzfile::base::utc() \endlink
//! creates a reference to an
//! object that refers to the basic \c UTC timezone.
//!
//! \par Local timezone name
//!
//! \code
//! std::string name=tzfile::base::localname();
//! \endcode
//!
//! \link tzfileBase::localname() tzfile::base::localname() \endlink
//! returns the name of the default system timezone. Although this string
//! can be passed to create() to create an actual timezone object, it's better
//! to invoke local():
//!
//! \code
//! tzfile tzlocal(tzfile::base::local());
//! \endcode
//!
//! \link tzfileBase::local() tzfile::base::local() \endlink returns a
//! reference to a timezone object for the local system timezone. The object
//! gets constructed the first time \c tzfile::base::local() gets called.
//! Subsequent calls to \c tzfile::base::local() retrieve another reference to
//! the same object.
//!
//! This is the optimum way to retrieve an object representing the local
//! system timezone.
//!
//! \par Enumerating available timezones
//!
//! \code
//! std::set&lt;std::string&gt; timezones;
//!
//! tzfile::base::enumerate(timezones);
//! \endcode
//!
//! enumerate() enumerates names of available timezones and places this list
//! into the given \c std::set&lt;std::string&gt;. Note: the existing contents
//! of the set, if any, are not cleared.

typedef ref<tzfileObj, tzfileBase> tzfile;

//! A nullable pointer reference to a timezone object.

//! \see tzfile

typedef ptr<tzfileObj, tzfileBase> tzfileptr;

//! Constant timezone reference

//! At present time, all methods in a timezone object are constant methods,
//! so this constant reference is equivalent in functionality to
//! #tzfile.

typedef const_ref<tzfileObj, tzfileBase> const_tzfile;

//! Pointer to a constant timezone object

//! \see const_tzfile

typedef const_ptr<tzfileObj, tzfileBase> const_tzfileptr;

//! A timezone object

//! \see #tzfile

class tzfileObj : virtual public obj {

	template<typename int_type>
	class frombe;

	//! Information about the local/alt timezone taken from a POSIX string.

	//! \internal
	//!
	class tzinfo {

	public:
		//! The timezone abbreviation

		std::string tzname;

		//! Offset from UTC, in seconds
		int32_t offset;

		//! Time of day, in seconds, when this timezone takes effect

		int32_t tod;

		//! How the day when this timezone takes effect is specified

		//! - '\\0' - day of the year, 1-365/366 is specified in \c d.
		//!
		//! - 'J' - day of the year, 1-365, is specified in \c d. Feb 29
		//! in a leap year is ignored.
		//!
		//! - 'M' - month/week/day is specified in \c m, \c w, and \\d.

		char start_format;
		int32_t m;	//!< Month (only when start_format='M')
		int32_t w;	//!< Week (only when start_format='M')
		int32_t d;	//!< Day

		//! Default constructor clears this object.
		tzinfo() noexcept : offset(0), tod(0), start_format(0),
			m(0), w(0), d(1)
		{
		}

		//! For a given year, compute the day that this timezone
		//! starts.

		ymd getDayOfGivenYear(//! Which year
				      int32_t year) const
			 LIBCXX_HIDDEN;

		//! Diagnostic method, only implemented in the regression test.

		template<typename OutputIterator>
		OutputIterator toString(OutputIterator iter)
			const LIBCXX_HIDDEN;
	};

	struct ttinfo_file;

	template<typename int_type>
	class LIBCXX_INTERNAL leaps_file;

	struct ttinfo_s;

	//! Internal structure

	struct __attribute__((packed)) leaps_s;

	//! Name of this timezone

	std::string name;

	//! Loaded timezone transitions

	vector<time_t> transitions;

	//! Loaded timezone transition index into ttinfo

	vector<unsigned char> ttinfo_idx;

	//! Loaded timezones

	vector<struct ttinfo_s> ttinfo;

	//! Loaded timezone name string buffer

	vector<char> tzstr;

	//! Loaded leap second information

	vector<leaps_s> leaps;

	//! Loaded POSIX timezone string, alternate timezone start
	tzinfo tz_alt_start;

	//! Loaded POSIX timezone string, alternate timezone end/standard start
	tzinfo tz_alt_end;

	//! Report a corrupted timezone file

	//! \internal
	//!
	static void badtzfileObj(const std::string &tzname)
		__attribute__((noreturn)) LIBCXX_INTERNAL;

	class leap_search;

	//! String that represents the local timezone

	static const char localnameid[] LIBCXX_INTERNAL;

	//! Local timezone file
	static const char localtime[] LIBCXX_INTERNAL;

	//! Initialize this object to the dummy UTC timezone
	void init_utc() LIBCXX_INTERNAL;

public:
	//! Default constructor

	tzfileObj();

	//! Return the name of this timezone

	std::string getName() const noexcept
	{
		return name;
	}

	friend class tzfileBase;

	//! Create a timezone object for the UTC timezone

	static tzfile utc();

	//! Determine the name of the default system timezone

	//! Currently, the default system timezone name is determined as
	//! follows:
	//!
	//! - contents of the \c TZ environment variable
	//!
	//! - parsed from the \c /etc/sysconfig/clock file
	//!
	//! - if everything fails, a dummy string that represents the local
	//! timezone file.

	static std::string localname();

	//! Load timezone

	tzfileObj(//! The timezone name

		  //! If the timezone name was obtained from localname(), and its
		  //! the special dummy string that's an alias for the local
		  //! timezone file, and this timezone file does not exist,
		  //! this object gets initialized with the dummy \c UTC
		  //! timezone data
		  const std::string &tzname);

	//! Default destructor
	~tzfileObj();

	//! Diagnostic function, implemented only in the regression test suite

	//! \internal
	//!
	template<typename OutputIterator>
	OutputIterator debugDump(OutputIterator iter,
				 std::ios_base &ios,
				 const const_locale &locale=locale::base::global())
		const;

	//! Diagnostic function, implemented only in the regression test suite

	//! \internal
	//!
	std::string
	debugDump(const const_locale &locale=locale::base::global())
		const;

	//! Diagnostic function, implemented only in the regression test suite

	//! \internal
	//!
	std::ostream &debugDump(std::ostream &o,
				const const_locale &locale
				=locale::base::global())
		const;

	//! Convert a time value to date+time using this timezone data

	//! \internal
	//! \see ymdhms

	ymdhmsBase compute_ymdhms(time_t timeValue) const;

	//! Convert a time value to date+time using this timezone data

	//! \internal
	//! \see ymdhms

	ymdhmsBase compute_ymdhms(const struct tm &timeValue)
		const;

	//! Convert a date and a time value to a combined date+time object

	//! \internal
	//! \see ymdhms

	ymdhmsBase compute_ymdhms(const ymd &ymdVal,
				  const hms &hmsVal) const;

	//! Convert a date+time object back to a time value

	//! \internal
	//! \see ymdhms

	time_t compute_time_t(const ymdhmsBase &) const;

private:

	//! Parse a timezone file

	//! \internal
	//!
	void load_file(const std::string &);

	//! Parse a timezone file

	//! \internal
	//!
	void load_file(fd &, const std::string &)
		LIBCXX_INTERNAL;

	//! Calculate the number of leap seconds that tick at the given time

	//! \internal
	int32_t compute_leapcnt(time_t timeValue,
				int &leapcnt) const noexcept LIBCXX_INTERNAL;

	class LIBCXX_INTERNAL compute_ymdhms_getoffind;

	//! Calculate alternate/standard timezone transition

	//! \internal
	//! For a given year, calculate when alternate/daylight time
	//! begins and ends, according to POSIX timezone specification
	//! in this timezone.

	void compute_transition(//! Year
				int32_t year,

				//! Alternate/daylight time starts now
				time_t &alt_start,

				//! Alternate/daylight time ends now
				time_t &alt_end) const;
	// Can't be internal, see testtzfile.C

	//! Compute timezone in effect for a given time

	//! \internal
	//! If the given time falls within the explicit timezone transition
	//! times specified for this timezone, the associated timezone is
	//! returned, otherwise the timezone is computed using the POSIX
	//! timezone string specified in the timezone file.

	template<typename CallbackType>
	void compute_transition(//! Compute timezone for this time
				time_t timeValue,

				//! Callback object method.
				//! The callback object's
				//! \c operator()(const ttinfo_s &)
				//! method gets returned.
				CallbackType &cb) const
		LIBCXX_INTERNAL;

	//! Parse header in the timezone file

	//! \internal
	//!
	template<typename time_t_type>
	int parse_tzhead(//! Timezone name, for debugging purposes
			 const std::string &tzname,

			 //! Header read from here
			 istream &i,

			 //! If \c true, timezone data is saved in this object,
			 //! if \c false, it gets ignored.

			 bool save) LIBCXX_INTERNAL;

	//! Parse POSIX timezone string

	//! \internal
	//!
	void parse_tzstring(const std::string &tzstring)
		LIBCXX_INTERNAL;

	//! Parse POSIX timezone string

	//! \internal
	//!
	static bool parse_tzname(std::string::const_iterator &,
				 std::string::const_iterator,
				 std::string &) noexcept LIBCXX_INTERNAL;
	//! Parse POSIX timezone string

	//! \internal
	//!
	static bool parse_tztod(std::string::const_iterator &,
				std::string::const_iterator,
				int32_t &n) noexcept LIBCXX_INTERNAL;
	//! Parse POSIX timezone string

	//! \internal
	//!
	static bool parse_tztodnum(std::string::const_iterator &,
				   std::string::const_iterator,
				   int32_t &n) noexcept LIBCXX_INTERNAL;
	//! Parse POSIX timezone string

	//! \internal
	//!
	static bool parse_tzwhen(std::string::const_iterator &,
				 std::string::const_iterator,
				 tzinfo &n) noexcept LIBCXX_INTERNAL;

public:
	//! Serialization function

	//! Only the timezone name gets serialized. When deserializing, the
	//! timezone name is loaded.

	template<typename iter_type>
	void serialize(iter_type &i)
	{
		i(name, 255);

		if (!i.serializing)
		{
			std::string namecpy(name);

			load_file(namecpy);
		}
	}
};

//! Refer to this class as \c tzfile::base or \c tzfileptr::base.

class tzfileBase : public ptrref_base {
public:

	//! Return the local system timezone name

	//! \see tzfileObj::locale()
	//!

	static std::string localname();

	//! Timezone database location

	//! \internal
	//!
	static std::string tzdir() noexcept;

	//! Enumerate available timezones

	static void enumerate(std::set<std::string> &);

	class localtz;
	class utctz;

	//! An internal singleton object holding the local timezone

	//! \internal
	//!
	static singleton<localtz> localtzInstance;

	//! Return a timezone object for the local system timezone

	//! This method returns a timezone object reference for the local
	//! system timezone. The object is created the first time this
	//! method is invoked. Subsequent invocations return a reference to the
	//! same object.

	static tzfile local();


	//! An internal singleton object holding the utc timezone

	//! \internal
	//!
	static singleton<utctz> utctzInstance;


	//! Create a timezone object for the local system timezone

	//! \see tzfileObj::utc()
	//!

	static tzfile utc();
};

inline std::string tzfileBase::localname()

{
	return tzfileObj::localname();
}
#if 0
{
#endif
}
#endif
