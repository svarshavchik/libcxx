/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_destroycallbackwait4obj_H
#define x_destroycallbackwait4obj_H

#include <x/obj.H>
#include <x/destroycallbackflagfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Destructor callback that waits for another object to get destroyed

class destroyCallbackFlagWait4Obj : virtual public obj {

	//! The flag for the other object.
	destroyCallbackFlag flag;

public:
	//! Constructor
	destroyCallbackFlagWait4Obj(const ref<obj> &other_obj);

	//! Destructor
	~destroyCallbackFlagWait4Obj() noexcept;

	//! Wait for another object to get destroyed, too.
	void destroyed();
};

#if 0
{
#endif
}

#endif

