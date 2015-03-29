/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_rwlockfwd_H
#define x_rwlockfwd_H

#include <x/ptrfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class rwlockObj;
class rwlockBase;

//! A reference to a rwlock

//! Construct read-write locks as follows:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::rwlock myRwlock=INSERT_LIBX_NAMESPACE::rwlock::create();
//! \endcode
//!
//! The read-write gets automatically destroyed
//! when the last reference to the read-write lock goes away.
//!
//! \link rwlockObj::readlock() myRwlock-&gt;readlock() \endlink,
//! \link rwlockObj::try_readlock() myRwlock-&gt;try_readlock() \endlink,
//! \link rwlockObj::try_readlock_until() myRwlock-&gt;try_readlock_until() \endlink, and
//! \link rwlockObj::try_readlock_for() myRwlock-&gt;try_readlock_for() \endlink,
//! lock the
//! rwlock for reading;
//! \link rwlockObj::writelock() myRwlock-&gt;writelock() \endlink,
//! \link rwlockObj::try_writelock() myRwlock-&gt;trywritelock() \endlink,
//! \link rwlockObj::try_writelock_until() myRwlock-&gt;try_writelock_until() \endlink, and
//! \link rwlockObj::try_writelock_for() myRwlock-&gt;try_writelock_for() \endlink,
//! locks the
//! rwlock for writing. They return a reference to a reference-counted object.
//! When the last reference to the lock object goes out of scope, the lock
//! object gets released.
//!
//! Multiple threads can lock the same read-write lock for reading.
//! Only one thread
//! can obtain a write lock on the read-write lock.

typedef ref<rwlockObj, rwlockBase> rwlock;

//! A nullable pointer to a read-write lock.

//! \see rwlock

typedef ptr<rwlockObj, rwlockBase> rwlockptr;

#if 0
{
#endif
}
#endif
