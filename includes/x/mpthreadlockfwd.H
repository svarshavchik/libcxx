/*
** Copyright 2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_mpthreadlockfwd_h
#define x_mpthreadlockfwd_h

#include <x/namespace.h>
#include <x/ptrfwd.H>
#include <mutex>

namespace LIBCXX_NAMESPACE {
#if 0
}
#endif

template<typename obj_type, typename mutex_type>
class mpthreadlockObj;

/*! Transferred lock on a mutex-protected object.

This is a handle for a logical
lock on a \ref mptobj "mptobj", that can be used
to construct another lock, in a different execution thread.

*/

template<typename obj_type, typename mutex_type=std::mutex>
using mpthreadlock=ref<mpthreadlockObj<obj_type, mutex_type>>;

//! A constant \ref mpthreadlock "transferred mutex-protected object lock".

//! \see mpthreadlock

template<typename obj_type, typename mutex_type=std::mutex>
using const_mpthreadlock=const_ref<mpthreadlockObj<obj_type, mutex_type>>;

//! A nullable pointer reference to a \ref mpthreadlock "transferred mutex-protected object lock".

//! \see mpthreadlock

template<typename obj_type, typename mutex_type=std::mutex>
using mpthreadlockptr=ptr<mpthreadlockObj<obj_type, mutex_type>>;

//! A nullable pointer reference to a const \ref mpthreadlock "transferred mutex-protected object lock".

//! \see mpthreadlock

template<typename obj_type, typename mutex_type=std::mutex>
using const_mpthreadlockptr=const_ptr<mpthreadlockObj<obj_type, mutex_type>>;

#if 0
{
#endif
}

#endif
