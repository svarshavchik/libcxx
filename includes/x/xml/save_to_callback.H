/*
** Copyright 2013-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_save_to_callback_H
#define x_xml_save_to_callback_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

//! Base class for an internal callback from libxml

//! \internal
class save_to_callback {
public:
	//! Constructor
	save_to_callback();

	//! Destructor
	~save_to_callback();

	//! Save a chunk of formatted XML
	virtual void save(const char *, int)=0;
};

#if 0
{
	{
#endif
	}
}
#endif
