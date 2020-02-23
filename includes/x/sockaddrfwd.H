/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sockaddrfwd_H
#define x_sockaddrfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class sockaddrObj;

//! A reference to a network address

typedef ref<sockaddrObj> sockaddr;

//! A reference to a constant network address

typedef const_ref<sockaddrObj> const_sockaddr;

//! A nullable pointer to a network address

typedef ptr<sockaddrObj> sockaddrptr;

//! A nullable pointer to a constant network address

typedef const_ptr<sockaddrObj> const_sockaddrptr;

#if 0
{
#endif
}
#endif
