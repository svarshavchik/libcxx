/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "eventfdobj.H"
#include "eventfd.H"
#include "sysexception.H"
#include "timespec.H"

#if HAVE_KQUEUE
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#endif

#include <errno.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

eventfdObj::~eventfdObj() noexcept
{
}

#if HAVE_LINUXSYSCALLS

int createeventfd() LIBCXX_INTERNAL;

int createeventfd()
{
	int fd;

	if ((fd=::eventfd(0, EFD_CLOEXEC)) < 0)
		throw SYSEXCEPTION("eventfd");

	return fd;
}

eventfdObj::eventfdObj() : fdObj(createeventfd())
{
}

void eventfdObj::event(eventfd_t nevents)
{
	if (eventfd_write(filedesc, nevents) < 0)
		throw SYSEXCEPTION("eventfd_write");
}

eventfd_t eventfdObj::event()
{
	eventfd_t cnt;

	if (eventfd_read(filedesc, &cnt) < 0)
		throw SYSEXCEPTION("eventfd_read");
	return cnt;
}

#else
#if HAVE_KQUEUE

#include "kqueuenb_internal.h"

int createeventfd() LIBCXX_INTERNAL;

int createeventfd()
{
	int fd;

	if ((fd=kqueue()) < 0)
		throw SYSEXCEPTION("kqueue");

	return fd;
}

eventfdObj::eventfdObj() : fdObj(createeventfd())
{
	struct kevent kev;

	KQUEUE_NONBLOCK_INIT;

	EV_SET(&kev, 0, EVFILT_USER, EV_ADD|EV_CLEAR, 0, 0, 0);

	if (kevent(filedesc, &kev, 1, NULL, 0, NULL) < 0)
	{
		::close(filedesc);
		filedesc= -1;
		throw SYSEXCEPTION("kevent");
	}
}

void eventfdObj::event(eventfd_t nevents)
{
	std::lock_guard<std::mutex> lock(objmutex);

	struct kevent kev;

	switch (KEVENT_POLL(filedesc, NULL, 0, &kev, 1)) {
	case -1:
		throw SYSEXCEPTION("kevent");
	case 0:
		break;
	case 1:
		{
			eventfd_t old=kev.fflags & 0x00FFFFFF;

			if ( (nevents+old < nevents) ||
			     ((nevents += old) & 0x00FFFFFF) != nevents)
			{
				errno=EINVAL;
				throw SYSEXCEPTION("eventfd_write");
			}
		}
		break;
	}

	EV_SET(&kev, 0, EVFILT_USER, 0, NOTE_TRIGGER|NOTE_FFCOPY|nevents, 3, 0);

	if (kevent(filedesc, &kev, 1, NULL, 0, NULL) < 0)
		throw SYSEXCEPTION("kevent");
}

eventfd_t eventfdObj::event()
{
	struct kevent kev;

	int rc=KEVENT_CALL(filedesc, NULL, 0, &kev, 1);

	if (rc < 0)
		throw SYSEXCEPTION("eventfd_read");

	if (rc == 0)
	{
		errno=EAGAIN;
		throw SYSEXCEPTION("eventfd_read");
        }

	return kev.fflags & 0x00FFFFFF;
}

#define KQUEUE_CLASS eventfdObj

KQUEUE_NONBLOCK_IMPL

#else


eventfdObj::eventfdObj() : fdObj(-1)
{
	errno=ENODEV;
	throw SYSEXCEPTION("eventfd");
}

void eventfdObj::event(eventfd_t nevents)
{
}

eventfd_t eventfdObj::event()
{
	return 0;
}

#endif
#endif

#if 0
{
#endif
}
