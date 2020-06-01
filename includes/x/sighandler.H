/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sighandler_H
#define x_sighandler_H

#include <x/functionalrefptr.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Signal handler

//! Handler installed by \ref install_sighandler "install_sighandler()".
typedef functionref<void (int)> sighandler_t;

//! Install a signal handler

//! The handler is a callable object that receives an int parameter.
//!
//! Installs a handler that gets invoked upon receipt of a signal. The
//! handler gets invoked in a separate thread. All signal handlers
//! get invoked by a single thread, so they should not perform time-
//! consuming tasks, but should terminate quickly.
//!
//! \return A mcguffin for the installed handler. The handler remains
//! installed as long as the mcguffin exists. When the mcguffin goes
//! out of scope, the signal handler is uninstalled.
//!
//! More than one handler may be installed for the same signal. All
//! signal handlers installed for a particular signal get invoked,
//! serially, upon receipt of a signal.
//!
//! When the mcguffin gets destroyed, the signal handler gets
//! deregistered.
//!
//! This implementation of sighandler handlers is not based on
//! non-thread aware <tt>sigaction(2)</tt> POSIX API, but rather the
//! Linux-specific
//! \ref signalfd "signalfd"(2). As noted, signals must be
//! blocked before these signal handlers take effect.
//!
//! Any thread that has a signal unblocked will prevent this sighandler
//! handler from getting invoked. Therefore, applications that
//! install these signal handlers should
//! \ref sigset "block the signals" as the first order of business
//! in main(), before any threads get started.

ref<obj> install_sighandler(//! Signal to register
			    int signum,
			    //! The signal's handler
			    const sighandler_t &handler);

#if 0
{
#endif
}

#endif
