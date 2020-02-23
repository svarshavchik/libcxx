/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_timertaskfwd_H
#define x_timertaskfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class timertaskObj;
class timertaskBase;

//! A regularly-scheduled task

//! \see ::timer
//!
//! This is a reference to a reference-counted object that represents a
//! regularly-executed task to be executed by a thread.

typedef ref<timertaskObj, timertaskBase> timertask;

//! A nullable reference pointer to a regularly-scheduled task.

//! \see timertask

typedef ptr<timertaskObj, timertaskBase> timertaskptr;

#if 0
{
#endif
}
#endif
