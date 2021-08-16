/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ondestroy.H"
#include "x/exception.H"
#include "weakinfo.H"
#include <cstdlib>
#include <iostream>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

ondestroy ondestroyBase::do_create(const ondestroy_cb_t &cb,
				   const ref<obj> &objArg,
				   bool autodestroy)
{
	return ptrref_base::objfactory<ondestroy>::create(cb, objArg,
							 autodestroy);
}

ondestroyObj::ondestroyObj(const ondestroy_cb_t &cbArg,
			   const ref<obj> &objArg,
			   bool autodestroyArg)
	: cb{cbArg}, wi{objArg->get_weak()},
	  wi_cb_iter{objArg->do_add_ondestroy(cbArg)},
	  autodestroy{autodestroyArg}
{
}

ondestroyObj::~ondestroyObj()
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

	if (!wi)
		return;

	auto &wiref= *wi;

	{
		weakinfoObj::weakinfo_data_t::lock wilock{wiref.weakinfo_data};

		if (wilock->objp) // If not, the list was emptied out.
			wilock->callback_list.erase(wi_cb_iter);
	}

	{
		// This makes sure that if the object is being destroyed, we
		// wait until it's completely destroyed

		std::lock_guard<std::recursive_mutex>
			templock(wiref.destroy_mutex);
	}

	wi=weakinfoptr{};
	cb=nullptr;

}
#if 0
{
#endif
}
