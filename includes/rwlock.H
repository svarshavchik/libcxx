/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef rwlock_H
#define rwlock_H

#include <x/rwlockobj.H>
#include <x/ref.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for \c INSERT_LIBX_NAMESPACE::rwlock and \c INSERT_LIBX_NAMESPACE::rwlockptr with some convenience typedefs

class rwlockBase : public ptrrefBase {

public:

	//! Refer to this type as \c INSERT_LIBX_NAMESPACE::rwlock::base::rlock (or INSERT_LIBX_NAMESPACE::rwlockptr::base::rlock).

	typedef rwlockObj::rlock rlock;

	//! Refer to this type as \c INSERT_LIBX_NAMESPACE::rwlock::base::rlockptr (or INSERT_LIBX_NAMESPACE::rwlockptr::base::rlockptr).

	typedef rwlockObj::rlockptr rlockptr;

	//! Refer to this type as \c INSERT_LIBX_NAMESPACE::rwlock::base::wlock (or INSERT_LIBX_NAMESPACE::rwlockptr::base::wlock).

	typedef rwlockObj::wlock wlock;

	//! Refer to this type as \c INSERT_LIBX_NAMESPACE::rwlock::base::wlockptr (or INSERT_LIBX_NAMESPACE::rwlockptr::base::wlockptr).

	typedef rwlockObj::wlockptr wlockptr;
};

#if 0
{
#endif
}
#endif
