/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef available_dests_h
#define available_dests_h

#include "libcxx_config.h"
#include "x/ref.H"
#include "x/obj.H"
#include "x/mpobj.H"
#include <cups/cups.h>

namespace LIBCXX_NAMESPACE {
	namespace cups {
#if 0
	};
};
#endif

class LIBCXX_HIDDEN available_destsObj : virtual public obj {

 public:

	// As long as cups.h insists on the library taking non-const
	// cups_dest_t parameters we will use locking to protect ourselves.

	typedef mpobj<cups_dest_t *, std::recursive_mutex> dests_t;

	dests_t dests;

	const decltype(cupsGetDests2(CUPS_HTTP_DEFAULT,
			       std::declval<cups_dest_t **>())) n_dests;

	available_destsObj();

	~available_destsObj();
};

typedef ref<available_destsObj> available_dests;

#if 0
{
	{
#endif
	};
};

#endif
