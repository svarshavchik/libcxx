/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_fdclientobj_H
#define x_http_fdclientobj_H

#include <x/http/fdclientimpl.H>
#include <x/obj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

//! A reference-counted fdclientiml

//! This class multiply inherits from fdclientimpl and obj.
//!
//! \see fdclient

class fdclientObj : public fdclientimpl, virtual public obj {

public:

	//! Constructor
	fdclientObj();

	//! Destructor
	~fdclientObj();

};


#if 0
{
	{
#endif
	}
}
#endif
