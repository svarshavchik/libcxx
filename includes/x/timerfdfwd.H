/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_timerfdfwd_H
#define x_timerfdfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class timerfdObj;

//! A file descriptor-based timer.

//! This object implements a timer based on a file descriptor object.
//! This is a reference to a reference-counted object that, once created,
//! can be passed around freely.
//!
//! Waiting for the timer to go off is equivalent to waiting for the underlying
//! file descriptor to become readable.
//!
//! As such, this timer object inherits
//! \link fdObj::epoll_add() epoll_add()\endlink,
//! \link fdObj::epoll_mod() epoll_mod()\endlink, and several other methods
//! from the underlying file descriptor object.
//!
//! \code
//! timerfd tfd(timerfd::create());
//! \endcode
//!
//! The create() function takes an optional parameter: \c CLOCK_MONOTONIC
//! (default) or \c CLOCK_REALTIME. See the \c timerfd_create(2) man page for
//! more informaton.
//!
//! \code
//! tfd->set(0, 1);
//! \endcode
//!
//! The \link timerfdObj::set() set()\endlink method arms the timer.
//! The first parameter is either 0 or \c TFD_TIMER_ABSTIME. The default 0
//! value specifies that the second parameter is an incremental time value;
//! \c TFD_TIMER_ABSTIME specifies that the second parameter is an absolute
//! timer value
//! (derived after previously consulting \link timerfdObj::gettime() gettime()\endlink.
//!
//! \code
//! tfd->set_periodically(1);
//! \endcode
//!
//! \ref timerfdObj::set_periodically "set_periodically"()
//! sets a recurring timer expiration interval. set() runs the timer once,
//! set_periodically() makes the timer run repeatedly.
//!
//! The time interval value to set() or set_periodically() may also be a
//! \ref timespecAnchor "timespec" object. This
//! allows specifying time intervals that include fractions of seconds:
//! \code
//! tfd->set(0, timespec(0, 1, 2));
//! \endcode
//!
//! This arms the timer to expire once, in one half of a second.
//!
//! set() and set_periodically() also have an optional parameter, a pointer
//! to a \ref timespecAnchor "timespec":
//! object:
//! \code
//!
//! struct itimerspec curtime;
//!
//! tfd->set(0, timespec(0, 1, 2), &curtime);
//! \endcode
//!
//! After arming the timer, the timer's previous status is placed in \c curtime,
//! which is described in the \c timer_gettime(2) man page.
//!
//! The timer's value may also be retrieved at any time using
//! \ref timerfdObj::gettime() "gettime"().
//!
//! \code
//!
//! struct itimerpec curtime=tfd->gettime();
//! \endcode
//!
//! \par Waiting for the timer to expire
//!
//! \code
//! uint64_t n=tfd->wait();
//! \endcode
//!
//! The \link timerfdObj::wait() wait() \endlink method returns the number of
//! times the timer object has expired, since the last time wait() was called
//! (for a recurring timer, a non-recurring timer expires at most once).
//! If the timer has not expired yet, and the underlying file descriptor is
//! a blocking file descriptor, wait() waits and does not return until it
//! expires. If the underlying file descriptor
//! a non-blocking file descriptor, wait() returns 0, immediately, if the timer
//! has not yet expired.
//!
//! After the underlying file descriptor is set non-blocking, it may be added
//! to an epoll set:
//!
//! \code
//! epoll epset;
//!
//! tfd->nonblock();
//! tfd->epoll(EPOLLIN, epset);
//! \endcode
//!
//! When this timer expires, the file descriptor receives an \c EPOLLIN event,
//! at which point wait() should be called to retrieve the expiration event,
//! and rearm the timer if it's a recurring timer.

typedef ref<timerfdObj> timerfd;

//! A nullable reference pointer to a timer file descriptor

//! \see timerfd
typedef ptr<timerfdObj> timerfdptr;

#if 0
{
#endif
}
#endif
