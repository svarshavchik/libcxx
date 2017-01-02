/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_timerfdobj_H
#define x_timerfdobj_H

#include <x/fdobj.H>
#include <x/timerfdfwd.H>
#include <x/exception.H>
#include <x/timespec.H>
#include <x/sys/timerfdsys.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A wrapper for a timerfd_create() file descriptor.

//! This is a subclass of timerfdObj that instantiates an event file
//! descriptor -- a Linux kernel-specific feature introduced in kernel 2.6.25.

class timerfdObj : public fdObj {

public:
	//! The constructor

	//! The constructor creates a new timer file descriptor.
	//!

	timerfdObj(//! \c CLOCK_REALTIME or \c CLOCK_MONOTONIC.
		   int clockid=CLOCK_MONOTONIC);

	//! The destructor

	//! Close the timer file descriptor before this object gets
	//! destructed.
	~timerfdObj();

	using fdObj::epoll_add;
	using fdObj::epoll_mod;
	using fdObj::closeonexec;
	using fdObj::nonblock;

	//! Set the timer to expire once.

	void set(//! 0, or \c TFD_TIMER_ABSTIME
		 int flags,

		 //! Timer expires at the given time
		 const timespec &it_value,

		 //! Previous timer value, if not NULL
		 struct itimerspec *curr_value=0);

	//! Set the timer to expire periodically.

	void set_periodically(//! Periodic timer value
			      const timespec &it_interval,

			      //! Previous timer value, if not NULL
			      struct itimerspec *curr_value=0);

	//! Cancel the timer.
	void cancel();

	//! An error occured arming the timer

	static void timerfd_settime_failed()
		__attribute__((noreturn)) LIBCXX_INTERNAL;

	//! Retrieve the current timer value

	struct itimerspec gettime();

	//! Wait for the timer to expire

	//! \return the number of times the timer has expired. If the
	//! underlying file descriptor is blocking, wait() always waits until
	//! the timer expires, if it has not done so already, and always
	//! returns a positive value. If the underlying file descriptor is
	//! a non-blocking file descriptor, wait() returns 0 if the timer
	//! has not yet expired.

	uint64_t wait();

	//! Expose the underlying file descriptor, for those who want it

	using fdObj::getFd;

private:

	//! Internal function defined only for a kqueue-based implementation

	//! \internal
	//!
	struct itimerspec kqueue_gettime_locked()
		LIBCXX_INTERNAL;

	//! Internal function defined only for a kqueue-based implementation

	//! \internal
	//!

	void get_curr_value(struct itimerspec *curr_value)
		LIBCXX_INTERNAL;

#include <x/sys/timerfdpriv.h>
#include <x/sys/kqueuenb.h>

};

#if 0
{
#endif
}
#endif
