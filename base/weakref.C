/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/weakptr.H"
#include "x/mutex.H"
#include "x/ref.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

#if 0
weakptr::weakptr(ptr<weakinfo> weakinfoArg)
	: ptr<weakinfo>(weakinfoArg)
{
}

weakptr::~weakptr() noexcept
{
}

ptr<obj> weakptr::getptr()
{
	weakinfo &wi= **this;

	muteximpl::mlock mutex_lock( wi.mutex );

	if (!wi.objp)
		return ptr<obj>();

	if (wi.objp->refadd(1) == 1)
	{
		// Race condition -- another thread is in the process of
		// destroying the last strong reference to this object, and
		// has invoked destroy() after obj::refcnt reached zero.

		// We need to wait() until destroy() locks this weakinfo
		// object, reads the obj::refcnt of 1, and signals this
		// thread, via the condition variable, to proceed.

		wi.cond.wait(wi.mutex);
	}

	return ptr<obj>(wi.objp);
}
#endif

#if 0
{
#endif
}
