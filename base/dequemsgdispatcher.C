/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/dequemsgdispatcher.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

dequemsgdispatcherObj::dequemsgdispatcherObj()
{
}

dequemsgdispatcherObj::~dequemsgdispatcherObj() noexcept
{
}

void dequemsgdispatcherObj::event(const dispatchablebase &msg)
{
	msgqueue_t::lock lock(msgqueue);

	lock.notify_all();
	lock->push_back(msg);
}

#if 0
{
#endif
}

