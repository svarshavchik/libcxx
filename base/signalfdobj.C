/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/signalfd.H"
#include "x/sysexception.H"
#if HAVE_KQUEUE
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#endif
#include "x/timespec.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

signalfdObj::~signalfdObj()
{
}

#if HAVE_LINUXSYSCALLS

int signalfdObj::createsignalfd()
{
	int filedesc;

	sigset empty;

	if ((filedesc=::signalfd(-1, &empty.mask, SFD_CLOEXEC)) < 0)
		throw SYSEXCEPTION("signalfd");

	return filedesc;
}

signalfdObj::signalfdObj() : fdObj(createsignalfd())
{
}

int signalfdObj::reset(const sigset &sigs)
{
	return ::signalfd(filedesc, &sigs.mask, nonblock() ? SFD_NONBLOCK:0);
}

void signalfdObj::capture(int signum)
{
	mpobj<metadata>::lock lock(meta);

	metadata &data= *lock;

	sigset newset=data.sigs + signum;

	if (reset(newset) < 0)
		throw SYSEXCEPTION("signalfd");

	data.sigs=newset;
}

void signalfdObj::uncapture(int signum)
{
	mpobj<metadata>::lock lock(meta);
	metadata &data= *lock;

	data.sigs -= signum;

	if (reset(data.sigs) < 0)
		throw SYSEXCEPTION("signalfd");
}

signalfdObj::getsignal_t signalfdObj::getsignal()
{

	getsignal_t sig=getsignal_t();

	read((char *)&sig, sizeof(sig));
	return sig;
}

#else
#if HAVE_KQUEUE

#include "kqueuenb_internal.h"

int signalfdObj::createsignalfd()
{
	int filedesc;

	if ((filedesc=kqueue()) < 0)
		throw SYSEXCEPTION("signalfd");

	return filedesc;
}

signalfdObj::signalfdObj() : fdObj(createsignalfd())
{
	KQUEUE_NONBLOCK_INIT;
}

void signalfdObj::capture(int signum)
{
	mpobj<metadata>::lock lock(meta);

	metadata &data= *lock;

	if (data.sigs & signum)
		return;

	struct kevent kev;

	EV_SET(&kev, signum, EVFILT_SIGNAL, EV_ADD, 0, 0, NULL);

	if (KEVENT_POLL(filedesc, &kev, 1, NULL, 0) < 0)
		throw SYSEXCEPTION("signalfd");

	data.sigs += signum;
}

void signalfdObj::uncapture(int signum)
{
	mpobj<metadata>::lock lock(meta);

	metadata &data= *lock;

	if (!(data.sigs & signum))
		return;

	struct kevent kev;

	EV_SET(&kev, signum, EVFILT_SIGNAL, EV_DELETE, 0, 0, NULL);

	if (KEVENT_POLL(filedesc, &kev, 1, NULL, 0) < 0)
		throw SYSEXCEPTION("signalfd");

	data.sigs -= signum;
}

signalfdObj::getsignal_t signalfdObj::getsignal()
{
	struct kevent kev;

	int rc=KEVENT_CALL(filedesc, NULL, 0, &kev, 1);

	if (rc < 0)
		throw SYSEXCEPTION("kevent");

	getsignal_t gs;

	gs.ssi_signo= rc == 0 ? 0:kev.ident;

	return gs;
}

#define KQUEUE_CLASS signalfdObj

KQUEUE_NONBLOCK_IMPL

#else
signalfdObj::signalfdObj()
{
	getsignal();
}

void signalfdObj::capture(int signum)
{
	getsignal();
}

void signalfdObj::uncapture(int signum)
{
	getsignal();
}

signalfdObj::getsignal_t signalfdObj::getsignal()
{
	errno=ENODEV;
	throw SYSEXCEPTION("signalfd");	
}

#endif
#endif



#if 0
{
#endif
}
