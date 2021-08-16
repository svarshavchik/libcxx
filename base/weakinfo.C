/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "weakinfo.H"
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

weakinfoObj::~weakinfoObj()
{
	weakinfo_data_t::lock lock{weakinfo_data};

	if (!lock->callback_list.empty())
	{
		std::cerr << "Internal error: weak reference callback list not empty in weakinfo destructor" << std::endl;
		abort();
	}
}

ptr<obj> weakinfoObj::getstrongref()
{
	weakinfo_data_t::lock mutex_lock{weakinfo_data};

	if (!mutex_lock->objp)
		return ptr<obj>();

	if (++mutex_lock->objp->refcnt == 1)
	{
		SELFTEST_HOOK();

		// Race condition -- another thread is in the process of
		// destroying the last strong reference to this object, and
		// has invoked destroy() after obj::refcnt reached zero.

		// We need to wait() until destroy() locks this weakinfo
		// object, reads the obj::refcnt of 1, and signals this
		// thread, via the condition variable, to proceed.

		// NOTE: we increment refcnt and clear destroy_aborted to
		// false while holding the mutex lock, so when
		// obj::destroy() detects it, it must be when wait()
		// unless the mutex and atomatically waits for the condition
		// variable, so destroy_method is guaranteed to get cleared
		// before obj::destroy() sets it to true.
		//
		// In case of multiple threads attempting to recover the
		// strong reference at the same time, only one execution
		// thread will increment refcnt from 0 to 1, and enter here.

		mutex_lock->destroy_aborted=false;
		try {
			mutex_lock.wait
				([&]
				 {
					 return mutex_lock->destroy_aborted;
				 });
		} catch (const exception &e)
		{
			--mutex_lock->objp->refcnt;
			throw;
		}
	}

	// Create a formal ptr, which will acquire its own reference count.
	// after which we can decrement the one we incremented temporarily,
	// above.

	ptr<obj> dummy{mutex_lock->objp};

	--mutex_lock->objp->refcnt;

	return dummy;
}

#if 0
{
#endif
}
