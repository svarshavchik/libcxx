/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_mutexfwd_H
#define x_option_mutexfwd_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/obj.H>

namespace LIBCXX_NAMESPACE::option {
#if 0
};
#endif

class mutexObj;
class valuebaseObj;

//! Implement a mutually-exclusive %option group

//! \see option
//!
typedef ref<mutexObj> mutex;

//! A nullable pointer to a mutually-exclusive %option group

//! \see option

typedef ptr<mutexObj> mutexptr;

#if 0
{
#endif
}
#endif
