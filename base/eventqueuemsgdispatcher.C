/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/eventqueuemsgdispatcher.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

eventqueuemsgdispatcherObj::eventqueuemsgdispatcherObj(const eventfd &eventfd)
	: msgqueue(msgqueue_t::create(eventfd))
{
}

eventqueuemsgdispatcherObj::~eventqueuemsgdispatcherObj()
{
}

void eventqueuemsgdispatcherObj::event(const dispatchablebase &msg)
{
	msgqueue->event(msg);
}

#if 0
{
#endif
}

