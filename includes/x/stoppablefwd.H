/*
** Copyright 2016-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_stoppablefwd_H
#define x_stoppablefwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class stoppableObj;
class stoppableBase;

//! A reference to a stoppable object

typedef ref<stoppableObj, stoppableBase> stoppable;

//! A nullable pointer reference to a stoppable object

typedef ptr<stoppableObj, stoppableBase> stoppableptr;

#if 0
{
#endif
}
#endif
