/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_dtdobj_H
#define x_xml_dtdobj_H

#include <x/obj.H>
#include <string>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

//! An external or an internal document type declaration.

class dtdObj : virtual public obj {

public:
	//! Constructor
	dtdObj();

	//! Destructor
	~dtdObj();

	//! Whether the doctype is defined
	virtual bool exists()=0;

	//! Return doctype's name
	virtual std::string name()=0;

	//! Return doctype's external ID
	virtual std::string external_id()=0;

	//! Return doctype's internal ID
	virtual std::string system_id()=0;
};

#if 0
{
#endif
}
#endif
