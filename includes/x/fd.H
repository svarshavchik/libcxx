/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fd_H
#define x_fd_H

#include <x/exceptionfwd.H>
#include <x/fdfwd.H>
#include <x/fdobj.H>
#include <x/threads/timer.H>
#include <x/threads/timertask.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif


template<typename duration_type>
ref<obj> fdObj::futimens_interval(const duration_type &interval)
{
	auto task_and_mcguffin=get_futimens_task();

	timer::base::global()->
		scheduleAtFixedRate(task_and_mcguffin.first, interval);
	return task_and_mcguffin.second;
}

#if 0
{
#endif
}
#endif
