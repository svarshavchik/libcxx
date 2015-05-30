/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_fdtlsclientobj_H
#define x_http_fdtlsclientobj_H

#include <x/http/fdtlsclientimpl.H>
#include <x/obj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {
		namespace http {
#if 0
		};
	};
};
#endif

//! A reference-counted fdtlsclientiml

//! This class multiply inherits from fdtlsclientimpl and obj.
//!
//! \see fdtlsclient

class fdtlsclientObj : public fdtlsclientimpl, virtual public obj {

public:

	//! Default constructor - no associated connection
	fdtlsclientObj();

	//! Destructor
	~fdtlsclientObj() noexcept;

};


#if 0
{
	{
		{
#endif
		}
	}
}
#endif