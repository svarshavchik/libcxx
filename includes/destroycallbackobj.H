/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_destroycallbackobj_H
#define x_destroycallbackobj_H

#include <x/obj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for destructor callbacks

//! Derive from destroyCallbackObj and implement the destroy() method, then
//! pass a reference to this object to obj::addOnDestroy() to install a callback
//! that gets invoked when the other object gets destroyed.

class destroyCallbackObj : virtual public obj {

public:
	friend class obj;
	friend class ondestroyObj;

	//! Default constructor

	destroyCallbackObj() noexcept;

	//! Default destructor
	~destroyCallbackObj() noexcept;

	//! The callback

	virtual void destroyed() noexcept=0;

	class onAnyDestroyCallbackObj;

};

#if 0
{
#endif
}

#endif

