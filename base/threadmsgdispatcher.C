/*
** Copyright 2016 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/threadmsgdispatcher.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

void threadmsgdispatcherObj::msgqueue_auto::event()
{
	(*this)->pop()->dispatch();
}

void threadmsgdispatcherObj::stop_me(threadmsgdispatcherObj *me)
{
	throw stopexception();
}

void threadmsgdispatcherObj::stop()
{
	sendevent(&threadmsgdispatcherObj::stop_me, this);
}

#if 0
{
#endif
}
