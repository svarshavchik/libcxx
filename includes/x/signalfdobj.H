/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_signalfdobj_H
#define x_signalfdobj_H

#include <x/fdobj.H>
#include <x/ptr.H>
#include <x/sigset.H>
#include <x/mpobj.H>
#include <x/sys/signalfdsys.h>

#include <map>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A wrapper for an signalfd() file descriptor.

//! This is a subclass of fdObj that instantiates a signal file
//! descriptor -- a Linux kernel-specific feature introduced in kernel 2.6.22.

class signalfdObj : private fdObj, virtual public obj {

	//! Internal file descriptor creator.

	static int createsignalfd() LIBCXX_INTERNAL;

	//! Important metadata

	class metadata {

	public:
		//! Current set of captured signals

		sigset sigs;
	};

	//! Mutex-protected metadata

	mpobj<metadata> meta;

	//! Update set of signals captured by the file descriptor

	int reset(const sigset &sigs) LIBCXX_INTERNAL;

#include <x/signal_def.h>

public:
	//! The constructor

	//! The constructor creates a new signal file descriptor.
	//!

	signalfdObj();

	//! The destructor

	//! Close the signal file descriptor, and unblock all signals
	//! that were blocked by this signal file descriptor (unless some
	//! particular signal remains blocked by some other file descriptor).

	~signalfdObj();

	//! Capture the requested signal

	//! Invoke signalfd() as appropriate.

	void capture(int signum);

	//! Stop capturing the requested signal

	//! The signal file descriptor will stop capturing the signal.
	//!

	void uncapture(int signum);

	//! The structure read by getsignal()

	typedef signalfd_siginfo getsignal_t;

	//! Read the next signal from the file descriptor

	//! If the underlying file descriptor is non-blocking, and no signal
	//! delivery is pending, the object returned by getsignal() will have
	//! ssi_signo set to 0.

	getsignal_t getsignal();

	using fdObj::nonblock;
	using fdObj::get_fd;
	using fdObj::closeonexec;
	using fdObj::epoll_add;
	using fdObj::epoll_mod;

#include <x/sys/kqueuenb.h>

};

#if 0
{
#endif
}
#endif
