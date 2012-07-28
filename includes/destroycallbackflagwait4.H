/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_destroycallbackflagwait4_H
#define x_destroycallbackflagwait4_H

#include <x/destroycallbackflag.H>
#include <x/destroycallbackflagwait4obj.H>

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
//! obj->addOnDestroy(destroyCallbackFlagWait4::create(mcguffin));
//! \endcode
//!
//! destroyCallbackFlagWait4 is a \ref ref "reference" to
//! a subclass of destroyCallbackObj. Its
//! constructor takes a reference to some mcguffin. When
//! destroyCallbackFlagWait4's destroyed() gets invoked, the thread stops and
//! waits until the mcguffin goes out of scope and gets destroyed, by some
//! other thread.

typedef ref<destroyCallbackFlagWait4Obj> destroyCallbackFlagWait4;

//! A nullable reference pointer to a destroyCallbackFlagWait4.

typedef ptr<destroyCallbackFlagWait4> destroyCallbackFlagWait4ptr;

#if 0
{
#endif
}

#endif

