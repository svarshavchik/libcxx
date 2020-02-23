/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mutexfwd_H
#define x_mutexfwd_H

#include <x/ptrfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class mutexObj;

//! A reference-counted mutex

//! This reference-counted mutex can acquire a lock in one thread that gets
//! released in another thread.
//!
//! Generally, a native mutex can be portably unlocked only by the same thread
//! that locked it. This is a reference to a reference-counted
//! object with a lock object that is also a reference-counted object, that
//! can be passed between threads, and released by a thread other than the
//! one that created it.

typedef ref<mutexObj> mutex;

//! A nullable pointer to a \ref mutex "mutex".

//! \see mutex

typedef ptr<mutexObj> mutexptr;

#if 0
{
#endif
}
#endif
