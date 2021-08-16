/*
** Copyright 2016-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_threadmsgdispatcher_H
#define x_threadmsgdispatcher_H

#include <x/threadmsgdispatcherobj.H>
#include <x/eventqueue.H>
#include <x/threads/run.H>
#include <x/destroy_callback.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Start a thread for an object that's derived from \ref threadmsgdispatcherObj "threadmsgdispatcherObj"

template<typename thread_object, typename ...Args>
static auto start_threadmsgdispatcher(const thread_object &object,
				      Args && ...args)
{
	// Construct the thread initialization mcguffin, and guard it
	// before returning.

	destroy_callback::base::guard guard;

	auto mcguffin=ref<obj>::create();

	guard(mcguffin);

	return run( object, ptr<obj>(mcguffin), std::forward<Args>(args)...);
}

#if 0
{
#endif
}
#endif
