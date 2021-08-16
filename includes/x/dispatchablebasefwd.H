/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_dispatchablebasefwd_H
#define x_dispatchablebasefwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class dispatchablebaseObj;

//! A reference to a dispatchable message base.

typedef ref<dispatchablebaseObj> dispatchablebase;

//! A nullable reference pointer to a dispatchable message base.

typedef ptr<dispatchablebaseObj> dispatchablebaseptr;

//! A reference to a dispatchable constant message base.

typedef const_ref<dispatchablebaseObj> const_dispatchablebase;

//! A nullable reference pointer to a dispatchable constant message base.

typedef const_ptr<dispatchablebaseObj> const_dispatchablebaseptr;


#if 0
{
#endif
}
#endif
