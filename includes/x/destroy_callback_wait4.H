/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_destroy_callback_wait4_H
#define x_destroy_callback_wait4_H

#include <x/destroy_callback.H>
#include <x/destroy_callback_wait4obj.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! When an object gets destroyed, wait for a second object to get destroyed too.

//! \code
//! ref<obj> mcguffin=ref<obj>::create();
//!
//! ref<myObject> obj=ref<myObject>::create();
//!
//! // ...
//!
//! obj->addOnDestroy(destroy_callback_wait4::create(mcguffin));
//! \endcode
//!
//! destroy_callback_wait4 is a \ref ref "reference" to
//! a subclass of destroyCallbackObj. Its
//! constructor takes a reference to some mcguffin. When
//! destroy_callback_wait4's destroyed() gets invoked, the thread stops and
//! waits until the mcguffin goes out of scope and gets destroyed, by some
//! other thread.

typedef ref<destroy_callback_wait4Obj> destroy_callback_wait4;

//! A nullable reference pointer to a destroy_callback_wait4.

typedef ptr<destroy_callback_wait4Obj> destroy_callback_wait4ptr;

#if 0
{
#endif
}

#endif
