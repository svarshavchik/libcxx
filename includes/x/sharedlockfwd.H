/*
** Copyright 2012-2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sharedlockfwd_H
#define x_sharedlockfwd_H

#include <x/ptrfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class sharedlockObj;
class sharedlockBase;

//! A reference to a reference-counted version of a \c std::shared_timed_mutex

//! Construct shared mutex objects as follows:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::sharedlock mySharedlock=INSERT_LIBX_NAMESPACE::sharedlock::create();
//! \endcode
//!
//! The shared lock gets automatically destroyed
//! when the last reference to the shared lock goes away.
//!
//! \link sharedlockObj::create_shared() mySharedlock-&gt;create_shared() \endlink,
//! \link sharedlockObj::try_shared() mySharedlock-&gt;try_shared() \endlink,
//! \link sharedlockObj::try_shared_until() mySharedlock-&gt;try_shared_until() \endlink, and
//! \link sharedlockObj::try_shared_for() mySharedlock-&gt;try_shared_for() \endlink,
//! lock the
//! sharedlock for reading;
//! \link sharedlockObj::create_unique() mySharedlock-&gt;create_unique() \endlink,
//! \link sharedlockObj::try_unique() mySharedlock-&gt;tryunique() \endlink,
//! \link sharedlockObj::try_unique_until() mySharedlock-&gt;try_unique_until() \endlink, and
//! \link sharedlockObj::try_unique_for() mySharedlock-&gt;try_unique_for() \endlink,
//! locks the
//! sharedlock for writing. They return a reference to a reference-counted object.
//! When the last reference to the lock object goes out of scope, the lock
//! object gets released.
//!
//! Multiple threads can lock the same shared lock for reading.
//! Only one thread
//! can obtain a write lock on the shared lock.

typedef ref<sharedlockObj, sharedlockBase> sharedlock;

//! A nullable pointer to a shared lock.

//! \see sharedlock

typedef ptr<sharedlockObj, sharedlockBase> sharedlockptr;

#if 0
{
#endif
}
#endif
