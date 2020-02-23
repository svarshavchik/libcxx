/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_cond_H
#define x_cond_H


#include <time.h>
#include <x/ptr.H>
#include <x/condobj.H>
#include <x/mutex.H>
#include <x/mlock.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class condObj;

//! A reference to a condition variable

//! Construct condion variables easily:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::cond myCond=INSERT_LIBX_NAMESPACE::cond::create();
//! \endcode
//!
//! The condition variable gets automatically destroyed
//! when the last reference to the cond goes away.
//!
//! Presumably there's a mutex reference nearby:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::mutex myMutex=INSERT_LIBX_NAMESPACE::mutex::create();
//! \endcode
//!
//! \ref condObj::wait() "myCond-&gt;wait(myMutex)",
//! \ref condObj::wait_until() "myCond-&gt;wait_until(lock, timeout)"
//! (where \c lock is a
//! \ref mlock "INSERT_LIBX_NAMESPACE::mlock"
//! and timeout is a time point) waits until
//! the condition variable is signaled by myMutex, which must be locked.
//!
//! \link condObj::notify_one() myCond-&gt;notify_one() \endlink signals this
//! condition variable, and
//! \link condObj::notify_all() myCond-&gt;notify_all() \endlink broadcasts it.

typedef ref<condObj> cond;

//! A pointer to a reference-counted condition variable

//! \see cond

typedef ptr<condObj> condptr;

#if 0
{
#endif
}
#endif
