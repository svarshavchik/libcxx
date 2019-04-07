/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_signalfdfwd_H
#define x_signalfdfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class signalfdObj;
class signalfdBase;

//! A signal file descriptor

//! A signal file descriptor captures signals that would normally invoke an
//! asynchronous signal handler. Instead, the signal gets captured by the
//! file descriptor.
//!
//! This object acts like a file descriptor. When it's readable, invoking
//! \ref signalfdObj::getsignal "getsignal()" returns a \c getsignal_t
//! structure, giving the information about the signal that was sent to the
//! process.
//!
//! \note
//! As noted in signalfd(2), the signals captured by a file descriptor should
//! be separately blocked. If any thread in a process has the signal unblocked,
//! the thread will receive a signal, rather than the file descriptor.
//! An application that intends to use signal file descriptors must block the
//! signals before starting any execution threads, which inherit the parent
//! thread's signal mask. Use \ref sigset "sigset" to block signals.

typedef ref<signalfdObj, signalfdBase> signalfd;

//! A nullable reference pointer to a signal file descriptor object.

//! \see signalfd

typedef ptr<signalfdObj, signalfdBase> signalfdptr;

#if 0
{
#endif
}

#endif
