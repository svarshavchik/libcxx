/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef sharedlock_H
#define sharedlock_H

#include <x/sharedlockobj.H>
#include <x/ref.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for \c INSERT_LIBX_NAMESPACE::sharedlock and \c INSERT_LIBX_NAMESPACE::sharedlockptr with some convenience typedefs

class sharedlockBase : public ptrref_base {

public:

	//! Refer to this type as \c INSERT_LIBX_NAMESPACE::sharedlock::base::shared (or INSERT_LIBX_NAMESPACE::sharedlockptr::base::shared).

	typedef sharedlockObj::shared shared;

	//! Refer to this type as \c INSERT_LIBX_NAMESPACE::sharedlock::base::sharedptr (or INSERT_LIBX_NAMESPACE::sharedlockptr::base::sharedptr).

	typedef sharedlockObj::sharedptr sharedptr;

	//! Refer to this type as \c INSERT_LIBX_NAMESPACE::sharedlock::base::unique (or INSERT_LIBX_NAMESPACE::sharedlockptr::base::unique).

	typedef sharedlockObj::unique unique;

	//! Refer to this type as \c INSERT_LIBX_NAMESPACE::sharedlock::base::uniqueptr (or INSERT_LIBX_NAMESPACE::sharedlockptr::base::uniqueptr).

	typedef sharedlockObj::uniqueptr uniqueptr;
};

#if 0
{
#endif
}
#endif
