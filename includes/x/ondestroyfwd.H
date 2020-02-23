/*
** Copyright 2014-2020 Double Precision, Inc.
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
class ondestroyBase;

//! A destructor reference

//! \see ptr
//!
typedef ref<ondestroyObj, ondestroyBase> ondestroy;

//! A nullable pointer to a destructor reference

typedef ptr<ondestroyObj, ondestroyBase> ondestroyptr;

#if 0
{
#endif
}
#endif
