/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_dispatchablebaseobj_H
#define x_dispatchablebaseobj_H

#include <x/namespace.h>
#include <x/obj.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Base class for a dispatchable message.

//! The base class defines a virtual dispatch() method.

class dispatchablebaseObj : virtual public obj {

public:
	dispatchablebaseObj();
	~dispatchablebaseObj();

	//! Dispatch the message

	//! Whatever "dispatch" means is defined in the subclass

	virtual void dispatch()=0;
};

#if 0
{
#endif
}
#endif
