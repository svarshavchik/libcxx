/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_destroycallbackflagobj_H
#define x_destroycallbackflagobj_H

#include <x/destroycallbackobj.H>
#include <x/mpobj.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A subclass of destroyCallbackObj whose destroyed() sets a bool flag

//! Invoke wait() to wait for destroyed() to be invoked
//!

class destroyCallbackFlagObj : public destroyCallbackObj {

	//! Flag, destroyed() has been invoked.

	mpcobj<bool> flag;

public:

	//! Constructor
	destroyCallbackFlagObj() noexcept;

	//! Destructor
	~destroyCallbackFlagObj() noexcept;

	//! The attached object has been destroyed
	void destroyed() noexcept;

	//! Wait for the attached object to be destroyed
	void wait();
};

#if 0
{
#endif
}

#endif

