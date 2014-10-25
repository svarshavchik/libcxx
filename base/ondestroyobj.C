/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "destroycallbackobj.H"
#include "ondestroy.H"
#include "exception.H"
#include <cstdlib>
#include <iostream>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

ondestroy ondestroyBase::do_create(const destroyCallback &cb,
				   const ref<obj> &objArg,
				   bool autodestroy)
{
	return ptrrefBase::objfactory<ondestroy>::create(cb, objArg,
							 autodestroy);
}

ondestroyObj::ondestroyObj(const ref<destroyCallbackObj> &cbArg,
			   const ref<obj> &objArg,
			   bool autodestroyArg)
	: cb(cbArg), wi(objArg->weak()),
	  wi_cb_iter(objArg->addOnDestroy(cbArg)),
	  autodestroy(autodestroyArg)
{
}

ondestroyObj::~ondestroyObj() noexcept
{
	if (autodestroy)
		try {
			cancel();
		} catch (const exception &e)
		{
			std::cerr << e << std::endl
				  << e->backtrace << std::flush;
			abort();
		}
}

void ondestroyObj::cancel()
{
	std::lock_guard<std::mutex> lock(mutex);

	if (wi.null())
		return;

	weakinfo &wiref= *wi;

	{
		std::lock_guard<std::mutex> wilock(wiref.mutex);

		if (wiref.objp)
			wiref.callback_list.erase(wi_cb_iter);
	}

	{
		// This makes sure that if the object is being destroyed, we
		// wait until it's completely destroyed

		std::lock_guard<std::recursive_mutex>
			templock(wiref.destroy_mutex);
	}

	wi=ptr<weakinfo>();
	cb=ptr<destroyCallbackObj>();

}
#if 0
{
#endif
}
