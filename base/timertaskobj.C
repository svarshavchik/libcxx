/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/threads/timertask.H"
#include "x/threads/timerobj.H"
#include "x/dequemsgdispatcher.H"
#include "x/logger.H"
#include "x/destroycallbackflag.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

#include "timerobj_internal.H"
#include "timertaskentry_internal.H"


timertaskObj::timertaskObj()
{
}

timertaskObj::~timertaskObj() noexcept
{
}

bool timertaskObj::install(const timertaskentry &te)
{
	std::lock_guard<std::mutex> lock(objmutex);

	if (timerentry.getptr().null())
	{
		timerentry=te;
		return true;
	}
	return false;
}

void timertaskObj::cancel()
{
	destroyCallbackFlag cb=destroyCallbackFlag::create();

	{
		auto p=({
				std::lock_guard<std::mutex> lock(objmutex);

				timerentry.getptr();
			});

		if (p.null())
			return;

		auto impl=p->impl.getptr();

		if (impl.null())
			return;

		ref<obj> mcguffin=ref<obj>::create();

		impl->canceltask(p, mcguffin);

		if (impl->samethread())
			return; // Soon.

		mcguffin->addOnDestroy(cb);
	}

	cb->wait();
}

ref<canceltimertaskObj> timertaskObj::autocancel()
{
	return ref<canceltimertaskObj>::create(ref<timertaskObj>(this));
}

canceltimertaskObj::canceltimertaskObj(const ref<timertaskObj> &taskArg)
	: task(taskArg)
{
}

canceltimertaskObj::canceltimertaskObj(ref<timertaskObj> &&taskArg)
	: task(std::move(taskArg))
{
}

canceltimertaskObj::~canceltimertaskObj() noexcept
{
	task->cancel();
}

#if 0
{
#endif
}
