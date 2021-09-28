/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "available_dests.H"

namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

available_destsObj::available_destsObj()
	: thr{cupsthread::create()},
	  internal_thread{start_threadmsgdispatcher(thr)}
{
}

available_destsObj::~available_destsObj()
{
	thr->shutdown();
	internal_thread->wait();
}

#if 0
{
#endif
}
