/*
** Copyright 2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakinfofwd_H
#define x_weakinfofwd_H

#include <x/ptrfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class LIBCXX_HIDDEN weakinfoObj;

//! An internal object implementing weak object tracking.

//! \internal

typedef ref<weakinfoObj> weakinfo;

//! A constant reference to a \c weakinfo

//! \see #weakinfo

typedef const_ref<weakinfoObj> const_weakinfo;

//! A pointer  to a \c weakinfo

//! \see #weakinfo

typedef ptr<weakinfoObj> weakinfoptr;

//! A constant pointer to a \c weakinfo

//! \see #weakinfo

typedef const_ptr<weakinfoObj> const_weakinfoptr;

#if 0
{
#endif
}
#endif
