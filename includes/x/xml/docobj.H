/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_docobj_H
#define x_xml_docobj_H

#include <x/obj.H>
#include <x/xml/dtdfwd.H>
#include <x/xml/newdtdfwd.H>
#include <x/xml/readlockfwd.H>
#include <x/xml/writelockfwd.H>
#include <x/xml/new_attribute.H>
#include <x/xml/new_element.H>
#include <set>
#include <vector>
#include <charconv>
#include <limits>
#include <functional>

namespace LIBCXX_NAMESPACE::xml {
#if 0
}
#endif

//! A parsed XML document.

class docObj : virtual public obj {

public:

	//! Constructor
	docObj();

	//! Destructor
	~docObj();

	//! Create a read lock on the document.

	//! Blocks if a write lock exists.
	virtual ref<readlockObj> readlock()=0;

	//! Create a write lock on the document.

	//! Blocks if a read or a write lock exists.
	virtual ref<writelockObj> writelock()=0;
};

#if 0
{
#endif
}
#endif
