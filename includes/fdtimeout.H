/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fdtimeout_H
#define x_fdtimeout_H

#include <x/fdtimeoutobj.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class fdtimeoutObj;

//! A reference to an immutable fdtimeoutObj

typedef const_ref<fdtimeoutObj> const_fdtimeout;

//! A reference to an fdtimeoutObj

typedef ref<fdtimeoutObj> fdtimeout;

//! A pointer to an immutable fdtimeoutObj

typedef const_ptr<fdtimeoutObj> const_fdtimeoutptr;

//! A pointer to an fdtimeoutObj

typedef ptr<fdtimeoutObj> fdtimeoutptr;
#if 0
{
#endif
}
#endif
