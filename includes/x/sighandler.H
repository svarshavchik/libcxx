/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sighandler_H
#define x_sighandler_H

#include <x/sighandlerobj.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class sighandlerBase;


//! A signal handler

//! Derive from sighandlerObj, and invoke
//! \ref sighandlerBase::install "INSERT_LIBX_NAMESPACE::sighandler::base::install()" to install
//! a handler that gets invoked upon receipt of a signal.

typedef ref<sighandlerObj, sighandlerBase> sighandler;

//! A nullable reference pointer to a sighandler handler object.

//! \see sighandler

typedef ptr<sighandlerObj, sighandlerBase> sighandlerptr;

//! Base class for signal handler pointers and references.

//! The base class defines \ref install sighandler::base::install() method.

class sighandlerBase {

public:
	//! Install a signal handler

	//! This function is typically referenced as
	//! \c INSERT_LIBX_NAMESPACE::sighandler::base::install().
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
	//! deregistered. If there are no more references to the handler, the
	//! handler object gets destroyed. However, the signal handler gets
	//! invoked by a separate thread, and if the mcguffin goes out of scope
	//! while the thread is executing the handler, the handler object gets
	//! destroyed after it returns.
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

	static ref<obj> install(//! Signal to register
				int signum,

				//! The signal's handler
				const sighandler &handler)
;
};

#if 0
{
#endif
}

#endif
