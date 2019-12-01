/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mlockfwd_H
#define x_mlockfwd_H

#include <x/ptrfwd.H>
#include <x/mutexfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class mlockObj;
class mlockBase;

//! A reference-counted mutex lock

//! \see mutex.

typedef ref<mlockObj> mlock;

//! A nullable pointer to a \ref mutex "mutex".

//! \see mutex

typedef ptr<mlockObj> mlockptr;

#if 0
{
#endif
}
#endif
