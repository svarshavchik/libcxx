/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_cups_destinationobj_H
#define x_cups_destinationobj_H

#include <x/cups/destinationfwd.H>
#include <x/cups/jobfwd.H>
#include <x/obj.H>
#include <string_view>
#include <unordered_set>

namespace LIBCXX_NAMESPACE {
	namespace cups {
#if 0
	}
}
#endif

//! Implementation object.

//! \see destination

class destinationObj : virtual public obj {

public:

	//! Constructor
	destinationObj() LIBCXX_HIDDEN;

	//! Destructor
	~destinationObj() LIBCXX_HIDDEN;

	//! Whether this option is supported.
	virtual bool supported(const std::string_view &option) const=0;

	//! Whether this option is valid.
	virtual bool supported(const std::string_view &option,
			       const std::string_view &value) const=0;

	//! Supported options.
	virtual std::unordered_set<std::string> supported_options() const=0;

	//! Supported option values.
	virtual option_values_t
	option_values(const std::string_view &option) const=0;

	//! Default destination values
	virtual option_values_t
	default_option_values(const std::string_view &option) const=0;

	//! Currently available destination values
	virtual option_values_t
	ready_option_values(const std::string_view &option) const=0;

	//! User-specified default configuration.
	virtual std::unordered_map<std::string, std::string>
	user_defaults() const=0;

	//! Create a new job.

	virtual job create_job()=0;
};

#if 0
{
	{
#endif
	}
}
#endif
