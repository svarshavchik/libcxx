/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_destroy_callbackfwd_H
#define x_destroy_callbackfwd_H

#include <x/ref.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class destroy_callbackObj;
class destroy_callbackBase;

//! A reference to a simple destructor callback implementation

typedef ref<destroy_callbackObj,
	    destroy_callbackBase> destroy_callback;

//! A pointer to a simple destructor callback implementation

typedef ptr<destroy_callbackObj,
	    destroy_callbackBase> destroy_callbackptr;


#if 0
{
#endif
}

#endif
