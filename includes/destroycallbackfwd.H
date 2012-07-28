/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_destroycallbackfwd_H
#define x_destroycallbackfwd_H

#include <x/ref.H>
#include <x/ptr.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class destroyCallbackObj;
class destroyCallbackBase;

//! A destructor callback

//! destroyCallbackObj is typically subclassed to implement a destructor
//! callback. destroyCallbackObj's subclass implements destroyed(), and
//! that's that. A custom create() function instantiates uses a template class
//! to instantiate subclass of destroyCallbackObj that invokes a functor.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj> someobject;
//!
//! // ...
//!
//! someobject->addOnDestroy(INSERT_LIBX_NAMESPACE::destroyCallback::create([]
//!                          {
//!                              // ... functor
//!                          });
//! \endcode
//!
//! The functor gets invoked as a destructor callback. A subclass of
//! destroyCallbackObj gets constructed that invokes the functor from its
//! destroyed() method.

typedef ref<destroyCallbackObj, destroyCallbackBase> destroyCallback;

//! A nullable pointer reference to a \ref destroyCallback "destroyCallback".

typedef ptr<destroyCallbackObj, destroyCallbackBase> destroyCallbackptr;

//! A reference to a constant \ref destroyCallback "destroyCallback".

typedef const_ref<destroyCallbackObj, destroyCallbackBase> const_destroyCallback;

//! A nullable pointer reference to a constant \ref destroyCallback "destroyCallback".

typedef const_ptr<destroyCallbackObj, destroyCallbackBase> const_destroyCallbackptr;

#if 0
{
#endif
}
#endif
