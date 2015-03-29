/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fdreadlimit_H
#define x_fdreadlimit_H

#include <x/fdreadlimitobj.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class fdreadlimitObj;

//! A reference to an fdreadlimitObj

typedef ref<fdreadlimitObj> fdreadlimit;

//! A reference to an immutable fdreadlimitObj

typedef const_ref<fdreadlimitObj> const_fdreadlimit;

//! A pointer to an fdreadlimitObj

typedef ptr<fdreadlimitObj> fdreadlimitptr;

//! A pointer to an immutable fdreadlimitObj

typedef const_ptr<fdreadlimitObj> const_fdreadlimitptr;

#if 0
{
#endif
}
#endif
