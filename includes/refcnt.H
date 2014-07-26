/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_refcnt_H
#define x_refcnt_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A reference count.

//! This class maintains a thread-safe counter, and defines two thread-safe
//! methods: refadd() and refget().

class refcnt {

	//! Current number of outstanding references

	//! \internal Use refadd() or refget() to access this
	//! counter.
	mutable long cnt;

public:
	//! Constructor

	//! The reference count gets initialized to 0 by default, however
	//! obj's constructor passes -1. This is used to
	//! detect attempts to ref(this) in the constructor (also the
	//! destructor. create() manually increments it to 0, before
	//! constructing the first ref, and ref's constructor kills the
	//! process if the new reference count is 0.

	inline refcnt(long cntArg=0) noexcept __attribute__((artificial))
		: cnt(cntArg) {}

	//! Destructor
	virtual inline ~refcnt() noexcept __attribute__((artificial)) {}

	//! Increment or decrement the reference counter

	//! This function adds \b inc to \link refcnt::cnt cnt \endlink atomicly,
	//! in a thread-safe manner.
	//! \return the new reference counter.
	//! \internal

	inline long refadd(//! Amount to add or subtract from \link refcnt::cnt cnt \endlink
			   long inc) noexcept __attribute__((artificial))
	{
		return __sync_fetch_and_add(&cnt, inc) + inc;
	}

	//! Return the current reference count

	//! This function returns the current value of \link refcnt::cnt cnt \endlink,
	//! which is retrieved in a thread-safe manner.
	//! \internal

	inline long refget() const noexcept __attribute__((artificial))
	{
		return __sync_fetch_and_add(&cnt, 0);
	}
};
#if 0
{
#endif
}
#endif
