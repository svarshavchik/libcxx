/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fdbasefwd_H
#define x_fdbasefwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class fdbaseObj;

//! A reference to a file descriptor-like based transport.

typedef ref<fdbaseObj> fdbase;

//! A reference to an immutable file descriptor-like based transport.

typedef const_ref<fdbaseObj> const_fdbase;

//! A nullable reference to a file descriptor-like based transport.

typedef ptr<fdbaseObj> fdbaseptr;

//! A nullable reference to an immutable file descriptor-like based transport.

typedef const_ptr<fdbaseObj> const_fdbaseptr;

#if 0
{
#endif
}
#endif
