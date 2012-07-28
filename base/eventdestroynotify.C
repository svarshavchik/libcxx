/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "eventdestroynotify.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

eventdestroynotify
eventdestroynotifyBase::newobj::create(const eventfd &eventfdArg,
				       const ref<obj> &objArg)
{
	auto r(ptrrefBase::objfactory<eventdestroynotify>::create());

	r->install(eventfdArg, objArg);

	return r;
}

eventdestroynotifyObj::eventdestroynotifyObj() noexcept : destroyedflag(false)
{
}

eventdestroynotifyObj::~eventdestroynotifyObj() noexcept
{
}

void eventdestroynotifyObj::install(const eventfd &eventfdArg,
				    const ref<obj> &objArg)
{
	notifyeventfd=eventfdArg;

	objArg->addOnDestroy(ref<destroyCallbackObj>(this));
}

void eventdestroynotifyObj::destroyed() noexcept
{
	eventfdptr e;

	{
		std::lock_guard<std::mutex> lock(objmutex);

		destroyedflag=true;

		e=notifyeventfd.getptr();
	}

	if (!e.null())
		e->event(1);
}
#if 0
{
#endif
}
