/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventfdfwd_H
#define x_eventfdfwd_H

#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class eventfdObj;
class exception;
class eventfdBase;

//! A reference to an event file descriptor.

//! \c eventfd encapsulates an event file descriptor.
//! See the Linux eventfd(2) man page for more information.
//! eventfd is a reference
//! to a reference-counted object. Once an event file descriptor gets
//! instantiated, its references maybe freely passed around:
//!
//! \code
//! eventfd ev;
//!
//! ev=eventfd::create();
//! \endcode

typedef ref<eventfdObj> eventfd;

//! A reference pointer to an event file descriptor

//! \see eventfd

typedef ptr<eventfdObj> eventfdptr;

#if 0
{
#endif
}

#endif
