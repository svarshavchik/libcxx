/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "cupsthread.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::cups::cupsthreadObj);

namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

void cupsthreadObj::run(ptr<obj> &threadmsgdispatcher_mcguffin)
{
        msgqueue_auto msgqueue(this);

	stopping=false;
        threadmsgdispatcher_mcguffin=nullptr;

	LOG_DEBUG("Enumerating destinations");

	n_dests=cupsGetDests2(CUPS_HTTP_DEFAULT, &dests);

	LOG_DEBUG("Enumerated " << n_dests << " destinations");

	while (!stopping)
		msgqueue.event();

	cupsFreeDests(n_dests, dests);
	LOG_DEBUG("Destroyed destinations");
}

void cupsthreadObj::dispatch_shutdown()
{
	stopping=true;
	LOG_DEBUG("Stopping");
}

void cupsthreadObj::in_thread(const functionref<void (cups_dest_t *,
						      decltype(n_dests))>
			      & closure)
{
	mpcobj<bool> done;

	access_destinations(closure, &done);

	mpcobj_lock lock{done};

	lock.wait([&] { return *lock; });
}

void cupsthreadObj::dispatch_access_destinations(const functionref<void
						 (cups_dest_t *,
						  decltype(n_dests))> &closure,
						 mpcobj<bool> *completed)
{
	try {
		closure(dests, n_dests);
	} catch (const exception &e)
	{
		LOG_ERROR(e);
	} catch (const std::exception &e)
	{
		LOG_ERROR(e.what());
	} catch (...)
	{
		LOG_ERROR("An unknown exceptionwas thrown");
	}

	mpcobj_lock lock{*completed};

	*lock=true;
	lock.notify_all();
}

#if 0
{
#endif
}
