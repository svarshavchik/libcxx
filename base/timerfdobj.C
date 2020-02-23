/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/timerfd.H"
#include "x/sysexception.H"
#if HAVE_KQUEUE
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include "kqueuenb_internal.h"
#endif

#include <errno.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

timerfdObj::~timerfdObj()
{
}

void timerfdObj::timerfd_settime_failed()
{
	throw SYSEXCEPTION("timerfd_settime");
}


#if HAVE_LINUXSYSCALLS

int createtimerfdesc(int clockid) LIBCXX_INTERNAL;

int createtimerfdesc(int clockid)
{
	int filedesc;

	if ((filedesc=timerfd_create(clockid, TFD_CLOEXEC)) < 0)
		throw SYSEXCEPTION("timerfd_create");

	return filedesc;
}

timerfdObj::timerfdObj(int clockid)
	: fdObj(createtimerfdesc(clockid))
{
}

struct itimerspec timerfdObj::gettime()
{
	struct itimerspec its;

	if (timerfd_gettime(filedesc, &its) < 0)
		throw SYSEXCEPTION("timerfd_gettime");
	
	return its;
}

uint64_t timerfdObj::wait()
{
	uint64_t n;

	ssize_t s=::read(filedesc, &n, sizeof(n));

	if (s < 0)
	{
		if (errno == EAGAIN)
			return 0;

		throw SYSEXCEPTION("read(timerfd)");
	}

	return n;
}

void timerfdObj::set(int flags,
		     const timespec &it_value,
		     struct itimerspec *curr_value)
{
	struct itimerspec dummy;
	struct itimerspec its;

	its.it_value=it_value;
	its.it_interval=timespec();

	if (!curr_value)
		curr_value= &dummy;

	if (timerfd_settime(filedesc, flags, &its, curr_value) < 0)
		timerfd_settime_failed();
}


void timerfdObj::set_periodically(const timespec &it_interval,
				  struct itimerspec *curr_value)

{
	struct itimerspec dummy;
	struct itimerspec its;

	its.it_value=it_interval;
	its.it_interval=it_interval;

	if (!curr_value)
		curr_value= &dummy;

	if (timerfd_settime(filedesc, 0, &its, curr_value) < 0)
		timerfd_settime_failed();
}

void timerfdObj::cancel()
{
	set(0, timespec());
}

#else
#if HAVE_KQUEUE

int createtimerfdesc() LIBCXX_INTERNAL;

int createtimerfdesc()
{
	int filedesc;

	if ((filedesc=kqueue()) < 0)
		throw SYSEXCEPTION("timerfd_create");

	return filedesc;
}

timerfdObj::timerfdObj(int clockid)
	: fdObj(createtimerfdesc())
{
	timerfd_clock=clockid;
	timerfd_set=false;
	KQUEUE_NONBLOCK_INIT;
}

struct itimerspec timerfdObj::kqueue_gettime_locked()

{
	struct itimerspec its;

	its.it_value=its.it_interval=timespec();

	if (timerfd_set)
	{
		// Punt for a periodic timer

		if (timerfd_interval.tv_sec == 0 &&
		    timerfd_interval.tv_nsec == 0)
		{
			timespec ts=timespec::getclock(timerfd_clock);

			if (ts < timerfd_value)
				its.it_value=timerfd_value - ts;
		}

		its.it_interval=timerfd_interval;
	}

	return its;
}

struct itimerspec timerfdObj::gettime()
{
	std::lock_guard<std::mutex> lock(objmutex);

	return kqueue_gettime_locked();
}

uint64_t timerfdObj::wait()
{
	struct kevent kev;

	int rc=KEVENT_CALL(filedesc, NULL, 0, &kev, 1);

	if (rc < 0)
		throw SYSEXCEPTION("kqueue");

	return rc == 0 ? 0:kev.data;
}

void timerfdObj::get_curr_value(struct itimerspec *curr_value)

{
	if (curr_value)
		*curr_value=kqueue_gettime_locked();

	struct kevent kev;

	// Clear any previous timer, if any.

	EV_SET(&kev, 0, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);

	KEVENT_POLL(filedesc, &kev, 1, NULL, 0);
	timerfd_set=false;
}

void timerfdObj::set(int flags,
		     const timespec &it_value,
		     struct itimerspec *curr_value)
{
	std::lock_guard<std::mutex> lock(objmutex);

	get_curr_value(curr_value);

	timespec ts=it_value;

	if (flags & TFD_TIMER_ABSTIME)
	{
		timespec now=timespec::getclock(timerfd_clock);

		if (now >= ts)
		{
			ts=timespec();
		}
		else
		{
			ts=it_value - now;
		}
	}

	struct kevent kev;

	EV_SET(&kev, 0, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0,
	       ts.tv_sec * 1000 + ts.tv_nsec / 1000000, NULL);

	if (kev.data == 0)
		kev.data=1;

	ts=it_value;

	if (!(flags & TFD_TIMER_ABSTIME))
		ts += timespec::getclock(timerfd_clock);

	if (KEVENT_POLL(filedesc, &kev, 1, NULL, 0) < 0)
		throw SYSEXCEPTION("kevent");

	timerfd_set=true;
	timerfd_value=it_value;
	timerfd_interval=timespec();
}


void timerfdObj::set_periodically(const timespec &it_interval,
				  struct itimerspec *curr_value)

{
	std::lock_guard<std::mutex> lock(objmutex);

	get_curr_value(curr_value);

	if (it_interval.tv_sec == 0 && it_interval.tv_nsec == 0)
	{
		errno=EINVAL;
		timerfd_settime_failed();
	}

	struct kevent kev;

	EV_SET(&kev, 0, EVFILT_TIMER, EV_ADD, 0,
	       it_interval.tv_sec * 1000 + it_interval.tv_nsec / 1000000, NULL);

	if (KEVENT_POLL(filedesc, &kev, 1, NULL, 0) < 0)
		throw SYSEXCEPTION("kevent");

	timerfd_set=true;
	timerfd_value=timespec();
	timerfd_interval=it_interval;
}

void timerfdObj::cancel()
{
	std::lock_guard<std::mutex> lock(objmutex);

	get_curr_value(NULL);
}

#define KQUEUE_CLASS timerfdObj

KQUEUE_NONBLOCK_IMPL

#else

timerfdObj::timerfdObj(int clockid)
{
	errno=ENODEV;
	throw SYSEXCEPTION("timerfd_create");
}

struct itimerspec timerfdObj::gettime()
{
	errno=ENODEV;
	timerfd_settime_failed();
}

uint64_t timerfdObj::wait()
{
	uint64_t n;

	errno=ENODEV;
	timerfd_settime_failed();
}

void timerfdObj::set(int flags,
		     const timespec &it_value,
		     struct itimerspec *curr_value)
{
	errno=ENODEV;
	timerfd_settime_failed();
}


void timerfdObj::set_periodically(const timespec &it_interval,
				  struct itimerspec *curr_value)

{
	errno=ENODEV;
	timerfd_settime_failed();
}

void timerfdObj::cancel()
{
	errno=ENODEV;
	timerfd_settime_failed();
}

#endif
#endif

#if 0
{
#endif
}
