/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_destroy_callback_wait4obj_H
#define x_destroy_callback_wait4obj_H

#include <x/obj.H>
#include <x/destroy_callbackfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Destructor callback that waits for another object to get destroyed

class destroy_callback_wait4Obj : virtual public obj {

	//! The flag for the other object.
	destroy_callback flag;

public:
	//! Constructor
	destroy_callback_wait4Obj(const ref<obj> &other_obj);

	//! Destructor
	~destroy_callback_wait4Obj();

	//! Wait for another object to get destroyed, too.
	void destroyed();
};

#if 0
{
#endif
}

#endif
