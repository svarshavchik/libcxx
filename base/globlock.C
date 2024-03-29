/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/globlock.H"
#include "x/sysexception.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

static std::mutex globmutex;

globlock::globlock()
{
	globmutex.lock();
}

globlock::~globlock()
{
	globmutex.unlock();
}

pid_t fork()
{
	globmutex.lock();

	pid_t p=::fork();

	if (p == 0)
	{
		return p;
	}

	globmutex.unlock();

	if (p < 0)
		throw SYSEXCEPTION("fork");
	return p;
}

#if 0
{
#endif
}
