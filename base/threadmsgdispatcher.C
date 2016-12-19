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

threadmsgdispatcherObj::msgqueue_objObj
::msgqueue_objObj(const ref<threadmsgdispatcherObj> &me,
		  const eventfd &fdArg)
	: msgqueue_auto(me, fdArg)
{
}

threadmsgdispatcherObj::msgqueue_objObj
::~msgqueue_objObj() noexcept
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
	: msgqueue_t(msgqueue_t::create(fdArg)),
	  me(me)
{
	// Install it in mgsqueueptr

	typename mpobj<msgqueueptr_t>::lock lock(me->msgqueueptr);

	*lock= static_cast<msgqueue_t>(*this);
}

threadmsgdispatcherObj::msgqueue_auto::~msgqueue_auto() noexcept
{
	// Remove myself from msgqueueptr
	typename mpobj<msgqueueptr_t>::lock lock(me->msgqueueptr);

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

void threadmsgdispatcherObj::stop()
{
	sendevent(&threadmsgdispatcherObj::stop_me, this);
}

#if 0
{
#endif
}
