/*
** Copyright 2012 Double Precision, Inc.
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
#include "kqueuenb_internal.h"
#endif
#include <fcntl.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

static int create_epoll_fd()
{
#if HAVE_LINUXSYSCALLS

	int nfd= ::epoll_create1(EPOLL_CLOEXEC);
#else
#if HAVE_KQUEUE

	int nfd=kqueue();

#else
	errno=ENODEV;
	int nfd= -1;
#endif
#endif

	if (nfd < 0)
		throw SYSEXCEPTION("epoll_create");

	return nfd;
}

epollObj::epollObj() : fdObj(-1)
{
#if HAVE_KQUEUE
	KQUEUE_NONBLOCK_INIT;
#endif

	filedesc=create_epoll_fd();
}

#if HAVE_KQUEUE
#define KQUEUE_CLASS epollObj

KQUEUE_NONBLOCK_IMPL
#endif

#if 0
{
#endif
}
