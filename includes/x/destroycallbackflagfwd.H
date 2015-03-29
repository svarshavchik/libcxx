/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_destroycallbackflagfwd_H
#define x_destroycallbackflagfwd_H

#include <x/ref.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class destroyCallbackFlagObj;
class destroyCallbackFlagBase;

//! A reference to a simple destructor callback implementation

typedef ref<destroyCallbackFlagObj,
	    destroyCallbackFlagBase> destroyCallbackFlag;

//! A pointer to a simple destructor callback implementation

typedef ptr<destroyCallbackFlagObj,
	    destroyCallbackFlagBase> destroyCallbackFlagptr;


#if 0
{
#endif
}

#endif

