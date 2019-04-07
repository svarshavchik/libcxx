/*
** Copyright 2014-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fixed_semaphore_H
#define x_fixed_semaphore_H

#include <x/fixed_semaphoreobj.H>
#include <x/fixed_semaphorefwd.H>
#include <x/ref.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! An acquired semaphore instance handle

//! \see fixed_semaphore
class fixed_semaphoreObj::acquiredObj : virtual public obj {

	//! How many were acquired
	size_t howmany;

	//! Whether they were really acquired at all.
	bool acquired;

	//! The semaphore that the instances might've been acquired from.
	const ref<fixed_semaphoreObj> acquired_from;

	//! The callback to release the semaphore instances.
	const ref<callbackBaseObj> callback;

public:
	friend class fixed_semaphoreObj;

	//! Constructor.

	//! \internal
	acquiredObj(const ref<fixed_semaphoreObj> acquired_fromArg,
		    const ref<callbackBaseObj> callbackArg) LIBCXX_HIDDEN;

public:
	//! Destructor
	~acquiredObj();
};

//! Base class for a \ref fixed_semaphore "fixed semaphore".

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::fixed_semaphore::base.

class fixed_semaphoreBase : public ptrref_base {

public:
	//! Convenience typedef.

	typedef fixed_semaphoreObj::acquiredObj acquiredObj;

	//! A reference to acquired semaphore instances.

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::fixed_semaphore::base::acquired
	//!
	//! \see fixed_semaphore
	typedef ref<acquiredObj> acquired;

	//! A nullable pointer reference to \ref acquired "acquired semaphore instances".

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::fixed_semaphore::base::acquiredptr
	typedef ptr<acquiredObj> acquiredptr;

	//! A reference to constant \ref acquired "acquired semaphore instances".

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::fixed_semaphore::base::const_acquired
	typedef const_ref<acquiredObj> const_acquired;

	//! A nullable pointer reference to constant \ref acquired "acquired semaphore instances".

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::fixed_semaphore::base::const_acquiredptr

	typedef const_ptr<acquiredObj> const_acquiredptr;
};

#if 0
{
#endif
}
#endif

