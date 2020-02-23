/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/epoll.H"
#include "x/fd.H"
#include "x/sysexception.H"
#include "x/timespec.H"
#if HAVE_KQUEUE
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#endif
#include <fcntl.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

#if HAVE_LINUXSYSCALLS

#else
#if HAVE_KQUEUE

#include "kqueuenb_internal.h"

int epoll_ctl_int(int epfd, int op, int fd, struct epoll_event *event)
	LIBCXX_INTERNAL;

int epoll_ctl_int(int epfd, int op, int fd, struct epoll_event *event)
{
	struct kevent kev[2];
	int n=0;

	if (event->events & EPOLLIN)
	{
		EV_SET(&kev[n], fd, EVFILT_READ,
		       op == EPOLL_CTL_DEL ? EV_DELETE
		       : (EV_ADD | (op & EPOLLONESHOT ?
				    EV_ONESHOT:0)), 0, 0,
		       event->data.ptr);
		++n;
	}

	if (event->events & EPOLLOUT)
	{
		EV_SET(&kev[n], fd, EVFILT_WRITE,
		       op == EPOLL_CTL_DEL ? EV_DELETE
		       : (EV_ADD | (op & EPOLLONESHOT ?
				    EV_ONESHOT:0)), 0, 0,
		       event->data.ptr);
		++n;
	}

	if (n == 0)
		return 0;

	return KEVENT_POLL(epfd, kev, n, NULL, 0);
}

int kqueue_epoll_ctl(int epfd, int op, int fd,
		     struct epoll_event *event) LIBCXX_INTERNAL;

int kqueue_epoll_ctl(int epfd, int op, int fd,
		     struct epoll_event *event)
{
	int rc;

	if (!(event->events & (EPOLLIN|EPOLLOUT)))
		return 0; // No-op

	rc=epoll_ctl_int(epfd, op, fd, event);

	if (op != EPOLL_CTL_DEL)
	{
		// ADD/MOD of only EPOLLIN or only EPOLLOUT gets mapped to a
		// DEL of the other one.

		struct epoll_event del_event= *event;

		del_event.events ^= (EPOLLIN|EPOLLOUT);
		(void)epoll_ctl_int(epfd, EPOLL_CTL_DEL, fd, &del_event);
	}
	return rc;
}

int kqueue_epoll_wait(int epfd, struct epoll_event *events,
		      int maxevents, int timeout) LIBCXX_INTERNAL;

int kqueue_epoll_wait(int epfd, struct epoll_event *events,
		      int maxevents, int timeout)
{
	struct kevent kev[maxevents];

	struct timespec ts;

	ts.tv_sec=timeout / 1000;

	ts.tv_nsec=timeout % 1000;
	ts.tv_nsec *= 1000000;

	int n=kevent(epfd, NULL, 0, kev, maxevents,
		     timeout < 0 ? NULL:&ts);

	if (n < 0)
		return -1;

	int i;

	for (i=0; i<n; ++i)
	{
		events[i].events=
			kev[i].filter == EVFILT_READ
			? EPOLLIN | (kev[i].flags & EV_EOF
				     ? EPOLLRDHUP:0)
			: kev[i].filter == EVFILT_WRITE
			? EPOLLOUT | (kev[i].flags & EV_EOF
				      ? EPOLLHUP:0)
			: 0;
		events[i].data.ptr=kev[i].udata;
	}
	return i;
}

#else
#endif
#endif


epollCallbackObj::epollCallbackObj()
{
}

epollCallbackObj::~epollCallbackObj()
{
}

epollObj::~epollObj()
{
}

void epollObj::epoll_ctl(int op, fdObj *fileObj, uint32_t eventmask)

{
	struct epoll_event nev=epoll_event();

	nev.events=eventmask;

	nev.data.ptr=reinterpret_cast<void *>(fileObj);

#if HAVE_LINUXSYSCALLS
	int rc=::epoll_ctl(filedesc, op, fileObj->filedesc, &nev);
#else
#if HAVE_KQUEUE
	int rc=kqueue_epoll_ctl(filedesc, op, fileObj->filedesc, &nev);
#else
	errno=ENODEV;
	int rc= -1;
#endif
#endif

	if (rc < 0)
		throw SYSEXCEPTION("epoll_ctl");

	if (op == EPOLL_CTL_ADD)
		fdcnt.fetch_add(1);
	if (op == EPOLL_CTL_DEL)
		fdcnt.fetch_add(-1);
}

void epollObj::epoll_wait(int timeout)
{
	std::vector<struct epoll_event> eventbuf;

	size_t cap=fdcnt;

	if (cap < 16)
		cap=16;

	eventbuf.resize(cap);

#if HAVE_LINUXSYSCALLS
	int n=::epoll_wait(filedesc, &eventbuf[0], cap, timeout);

#else
#if HAVE_KQUEUE
	int n=kqueue_epoll_wait(filedesc, &eventbuf[0], cap, timeout);
#else
	errno=ENODEV;
	int n= -1;
#endif
#endif

	if (n < 0)
	{
		if (errno == EINTR)
			return;

		throw SYSEXCEPTION("epoll_wait");
	}

	int i;

	for (i=0; i<n; i++)
	{
		fd filedesc(reinterpret_cast<fdObj *>(eventbuf[i].data.ptr));

		filedesc->epoll_callback->event(filedesc, eventbuf[i].events);
	}
}

#if 0
{
#endif
}
