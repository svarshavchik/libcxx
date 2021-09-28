/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef available_dests_h
#define available_dests_h

#include "libcxx_config.h"
#include "x/ref.H"
#include "x/obj.H"
#include "x/mpobj.H"
#include "x/threads/run.H"
#include "cupsthread.H"
#include <cups/cups.h>

namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

class LIBCXX_HIDDEN available_destsObj;

class available_destsObj : virtual public obj {

public:
	// Object that runs a dedicated CUPS execution thread

	const cupsthread thr;

private:

	// The internal execution thread.
	runthread<void> internal_thread;

public:

	// Start the execution thread and obtain a list of all print destinations.

	available_destsObj();

	// The destructor stops the CUPS execution thread
	~available_destsObj();
};

typedef ref<available_destsObj> available_dests;

#if 0
{
#endif
}

#endif
