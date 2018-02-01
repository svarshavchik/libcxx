/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_cups_availableobj_H
#define x_cups_availableobj_H

#include <x/cups/availablefwd.H>
#include <x/cups/destinationfwd.H>
#include <x/obj.H>
#include <string>
#include <unordered_map>

namespace LIBCXX_NAMESPACE {
	namespace cups {
#if 0
	}
}
#endif

//! Implementation object.

//! \see available

class availableObj : virtual public obj {

public:

	//! Constructor
	availableObj() LIBCXX_HIDDEN;

	//! Destructor
	~availableObj() LIBCXX_HIDDEN;

	//! Name
	virtual std::string name() const=0;

	//! Whether this is the default available printer.
	virtual bool is_default() const=0;

	//! Whether this printer was discovered on the network (remote printer).
	virtual bool is_discovered() const=0;

	//! Options
	virtual std::unordered_map<std::string, std::string> options() const=0;

	//! Return detailed information about this available printer.

	virtual const_destination info() const=0;

	//! Return detailed information about this available printer.

	virtual destination info()=0;
};

#if 0
{
	{
#endif
	}
}
#endif
