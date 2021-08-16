/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_xpathobj_H
#define x_xml_xpathobj_H

#include <x/obj.H>
#include <x/xml/xpathfwd.H>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

//! The result of get_xpath().

//! get_xpath() returns a \ref xpath "INSERT_LIBX_NAMESPACE::xml::xpath",
//! which is a reference to this object.

class xpathObj : virtual public obj {

public:
	//! Constructor
	xpathObj();

	//! Destructor
	~xpathObj();

	//! How many nodes in the resulting nodeset.
	virtual size_t count() const=0;

	//! Position the lock to the only node in the nodeset.

	virtual void to_node()=0;

	//! Position the lock to one of the nodes in the nodeset.

	virtual void to_node(//! This is 1-based, not 0-based index.
			     size_t n)=0;

	//! Evaluate the xpath expression as a bool.
	virtual bool as_bool() const=0;

	//! Evaluate the xpath expression as a number.
	virtual double as_number() const=0;

	//! Evaluate the xpath expression as a string.
	virtual std::string as_string() const=0;
};

#if 0
{
#endif
}
#endif
