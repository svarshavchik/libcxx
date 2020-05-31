/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakinfobaseobj_H
#define x_weakinfobaseobj_H

#include <x/namespace.h>
#include <x/ptrfwd.H>
#include <x/obj.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Internal implementation object used by weak pointers

//! \see weakptr

class weakinfobaseObj : virtual public obj {

	//! Retrieve a strong reference to the object.
	virtual ptr<obj> getstrongref()=0;

public:
	template<class weakType> friend class weakptr;

};

#if 0
{
#endif
}
#endif
