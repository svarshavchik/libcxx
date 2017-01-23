/*
** Copyright 2016-2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/threadmsgdispatcher.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

threadmsgdispatcherObj::msgqueue_objObj
::msgqueue_objObj(const ref<threadmsgdispatcherObj> &me,
		  const eventfd &fdArg)
	: msgqueue_auto(me, fdArg)
{
}

threadmsgdispatcherObj::msgqueue_objObj
::~msgqueue_objObj()
{
}

threadmsgdispatcherObj::msgqueue_auto
::msgqueue_auto(threadmsgdispatcherObj *me,
		const eventfd &fdArg)
	: msgqueue_auto(ref<threadmsgdispatcherObj>(me), fdArg)
{
}

threadmsgdispatcherObj::msgqueue_auto
::msgqueue_auto(const ref<threadmsgdispatcherObj> &me,
		const eventfd &fdArg)
	: msgqueue_auto(msgqueue_t::create(fdArg), me->msgqueueptr)
{
}

//! Construct an auxiliary queue
threadmsgdispatcherObj::msgqueue_auto
::msgqueue_auto(threadmsgdispatcherObj *me,
		active_queue_t &auxqueue)
	: msgqueue_auto(ref<threadmsgdispatcherObj>(me), auxqueue)
{
}

threadmsgdispatcherObj::msgqueue_auto
::msgqueue_auto(const ref<threadmsgdispatcherObj> &me,
		active_queue_t &auxqueue)
	: msgqueue_auto(msgqueue_t::create(me->get_msgqueue()->getEventfd()),
			auxqueue)
{
}

threadmsgdispatcherObj::msgqueue_auto
::msgqueue_auto(const msgqueue_t &q,
		active_queue_t &active_queue)
	: msgqueue_t(q),
	  active_queue(active_queue)
{
	typename mpobj<msgqueueptr_t>::lock lock(active_queue);

	*lock=*this;
}

threadmsgdispatcherObj::msgqueue_auto::~msgqueue_auto()
{
	// Remove myself from msgqueueptr
	typename mpobj<msgqueueptr_t>::lock lock(active_queue);

	*lock=msgqueueptr_t();
}

void threadmsgdispatcherObj::msgqueue_auto::event()
{
	(*this)->pop()->dispatch();
}

void threadmsgdispatcherObj::stop_me(threadmsgdispatcherObj *me)
{
	throw stopexception();
}

void threadmsgdispatcherObj::process_events(active_queue_t &auxqueue)
{
	auto my_q=get_msgqueue();
	auto other_q=get_msgqueue(auxqueue);

	if (my_q.null() || other_q.null())
		return;

	my_q->transfer_events(other_q);
}

void threadmsgdispatcherObj::stop()
{
	sendevent(&threadmsgdispatcherObj::stop_me, this);
}

#if 0
{
#endif
}
