/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_destroy_callbackobj_H
#define x_destroy_callbackobj_H

#include <x/obj.H>
#include <x/mpobj.H>
#include <x/destroy_callbackfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Implements a destroyed() callback that sets an internal flag.

//! Invoke wait() to wait for destroyed() to be invoked.

class destroy_callbackObj : virtual public obj {

	//! Flag, destroyed() has been invoked.

	mpcobj<bool> flag;

public:

	//! Constructor
	destroy_callbackObj() noexcept;

	//! Destructor
	~destroy_callbackObj();

	//! The attached object has been destroyed
	void destroyed();

	//! Wait for the attached object to be destroyed
	void wait();
};

#if 0
{
#endif
}

#endif
