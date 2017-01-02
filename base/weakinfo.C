/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/weakptr.H"
#include "x/obj.H"
#include "x/mutex.H"

#include <cstdlib>
#include <iostream>

#ifndef SELFTEST_HOOK
#define SELFTEST_HOOK() do { } while (0)
#endif

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

weakinfo::~weakinfo()
{
	if (!callback_list.empty())
	{
		std::cerr << "Internal error: weak reference callback list not empty in weakinfo destructor" << std::endl;
		abort();
	}
}

ptr<obj> weakinfo::getstrongref() const
{
	std::unique_lock<std::mutex> mutex_lock(mutex);

	if (!objp)
		return ptr<obj>();
	if (objp->refadd(1) == 1)
	{
		SELFTEST_HOOK();

		// Race condition -- another thread is in the process of
		// destroying the last strong reference to this object, and
		// has invoked destroy() after obj::refcnt reached zero.

		// We need to wait() until destroy() locks this weakinfo
		// object, reads the obj::refcnt of 1, and signals this
		// thread, via the condition variable, to proceed.

		try {
			cond.wait(mutex_lock);
		} catch (const exception &e)
		{
			objp->refadd(-1);
			throw;
		}
	}

	ptr<obj> dummy(objp);

	objp->refadd(-1);

	return dummy;
}

#if 0
{
#endif
}
