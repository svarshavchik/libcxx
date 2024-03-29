/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_cups_destinationfwd_H
#define x_cups_destinationfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>
#include <x/cups/collectionfwd.H>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <variant>
#include <string>

namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

class destinationObj;

//! Printer destination.

//! This object encapsulates CUPS's \c cups_dinfo_t object. Its methods return
//! detailed information about a particular print destination. The following
//! methods may throw exceptions if the are unable to receive a response from
//! the print destination. This object maintains an internal connection, and
//! the application should not store this object for a lengthy period of time,
//! before using it.
//!
//! \code
//! std::unordered_set<std::string> all_options=destination->supported_options();
//!
//! bool flag=destination->supported("output-bin");
//!
//! bool flag=destination->supported("output-bin", "face-up");
//! \endcode
//!
//! supported() checks whether the destination supports the given option
//! and a particular value for the option. This executes
//! cupsCheckDestSupported(). all_options() enumerates all options supported
//! by the destination, see https://www.cups.org/doc/cupspm.html#detailed-destination-information
//! for more information.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::cups::option_values_t values=destination->option_values("print-color-mode");
//!
//! INSERT_LIBX_NAMESPACE::cups::option_values_t values=destination->default_option_values("print-color-mode");
//!
//! INSERT_LIBX_NAMESPACE::cups::option_values_t values=destination->ready_option_values("print-color-mode");
//! \endcode
//!
//! option_values() enumerates the supported values for the given option.
//! default_option_values() returns the default value(s).
//! ready_option_values() returns the currently available value(s) for the
//! destination. They execute cupsFindDestSupported(), cupsFindDestDefault(),
//! and cupsFindDestReady() to get this information, and return a
//! \ref option_values_t "option_values_t" object.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::cups::option_values_t values=destination->option_values("{unicode}finishings");
//! \endcode
//!
//! Specifying a "{unicode}" prefix for some options uses unicode strings
//! instead of localized strings, for the returned values.
//!
//! \code
//! std::unordered_map<std::string, std::string> values=destination->user_defaults();
//! \endcode
//!
//! user_defaults() returns the user-specified default configuration for this
//! print destination. This returns the list of options from \c cups_dinfo_t.
//!
//! \code
//! #include <INSERT_LIBX_NAMESPACE/cups/job.H>
//!
//! INSERT_LIBX_NAMESPACE::cups::job job=destination->create_job();
//! \endcode
//!
//! create_job() creates a new print job to this destination.

typedef ref<destinationObj> destination;

//! A nullable pointer reference to a destination printer.

typedef ptr<destinationObj> destinationptr;

//! A constant destination printer object.

typedef const_ref<destinationObj> const_destination;

//! A nullable pointer reference to a const destination printer object.

typedef const_ptr<destinationObj> const_destinationptr;

//! Destination resultion
//!
//! \see option_values_t
struct resolution {
	enum units_t { per_inch, per_cm, unknown } units;
	int xres;
	int yres;

	operator std::string() const;
};

//! Option values

//! A variant expressing the values of an option. The value depends on the
//! option:
//!
//! - no value (monostate): option does not exist or has no value
//!
//! - a map of strings: text string option values. The text strings are the
//! map keys, the values are the localized string for the option's value. If
//! the option's name has a "{unicode}" prefix, then the map value is a
//! std::u32string. If no localized string is available, the map value is the
//! same as the map key.
//!
//! - a map of integers: enumerated value. The enumerated values are the map
//! keys, the map value is a label for the enumerated values. The map label
//! is a std::u32string if the option's name has a "{unicode}" prefix.
//!
//! - a set of integers: possible integer values for the option.
//!
//! - a vector of integer tuple pairs: possible lower/upper ranges for the option.
//! - a set of bool values: this is used for boolean options
//!
//! - a vector of \ref resolution "resolutions" specifying available print
//! resolutions.
//!
//! - an option \ref collection "collection group".
//!
//! \see destination

typedef std::variant< std::monostate,
		      std::unordered_map<std::string, std::string>,
		      std::unordered_map<std::string, std::u32string>,
		      std::unordered_map<int, std::string>,
		      std::unordered_map<int, std::u32string>,
		      std::unordered_set<int>,
		      std::unordered_set<bool>,
		      std::vector<resolution>,
		      std::vector<std::tuple<int, int>>,
		      std::vector<const_collection>> option_values_t;
#if 0
{
#endif
}
#endif
