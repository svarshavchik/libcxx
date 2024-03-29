/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/threads/timertask.H"
#include "x/threads/timerobj.H"
#include "x/threadmsgdispatcher.H"
#include "x/logger.H"
#include "x/destroy_callback.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

#include "timerobj_internal.H"
#include "timertaskentry_internal.H"


timertaskObj::timertaskObj()
{
}

timertaskObj::~timertaskObj()
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
	destroy_callback cb=destroy_callback::create();

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

		mcguffin->ondestroy([cb] { cb->destroyed(); });
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

canceltimertaskObj::~canceltimertaskObj()
{
	task->cancel();
}

#if 0
{
#endif
}
