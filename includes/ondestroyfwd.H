/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ondestroyfwd_H
#define x_ondestroyfwd_H

#include <x/ptrfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class ondestroyObj;

//! A destructor reference

//! \see ptr
//!
typedef ref<ondestroyObj> ondestroy;

//! A nullable pointer to a destructor reference

typedef ptr<ondestroyObj> ondestroyptr;

#if 0
{
#endif
}
#endif
