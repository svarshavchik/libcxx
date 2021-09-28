/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef cupsthread_h
#define cupsthread_h

#include "libcxx_config.h"
#include "x/ref.H"
#include "x/obj.H"
#include "x/threadmsgdispatcher.H"
#include "x/cups/destination.H"
#include "x/logger.H"
#include "x/functional.H"
#include <cups/cups.h>

namespace LIBCXX_NAMESPACE::cups {

#if 0
};
#endif

class LIBCXX_HIDDEN cupsthreadObj;

class cupsthreadObj : public threadmsgdispatcherObj {

	LOG_CLASS_SCOPE;

	bool stopping;

	cups_dest_t *dests;
	decltype(cupsGetDests2(CUPS_HTTP_DEFAULT,
			       std::declval<cups_dest_t **>())) n_dests;

public:

	// Execution thread
	void run(ptr<obj> &threadmsgdispatcher_mcguffin);

	// Invoke a closure in the context of the execution thread.

	void in_thread(const functionref<void (cups_dest_t *,
					       decltype(n_dests))> & closure);

#include "cupsthread.inc.H"
};

typedef ref<cupsthreadObj> cupsthread;

#if 0
{
#endif
}

#endif
