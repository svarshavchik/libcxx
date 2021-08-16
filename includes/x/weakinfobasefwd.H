/*
** Copyright 2020-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakinfobasefwd_H
#define x_weakinfobasefwd_H

#include <x/ptrfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class LIBCXX_HIDDEN weakinfobaseObj;

//! Interface for retrieving a strong reference from a weakly-held one.

//! Used by weakptr to recover a strong reference.
//!
//! \internal

typedef ref<weakinfobaseObj> weakinfobase;

//! A constant reference to a \c weakinfobase

//! \see #weakinfobase

typedef const_ref<weakinfobaseObj> const_weakinfobase;

//! A pointer  to a \c weakinfobase

//! \see #weakinfobase

typedef ptr<weakinfobaseObj> weakinfobaseptr;

//! A constant pointer to a \c weakinfobase

//! \see #weakinfobase

typedef const_ptr<weakinfobaseObj> const_weakinfobaseptr;

#if 0
{
#endif
}
#endif
