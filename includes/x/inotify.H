/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_inotify_H
#define x_inotify_H

#include <x/inotifyfwd.H>
#include <x/inotifyobj.H>
#include <x/callback.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename functor_type>
ref<obj> inotifyObj::create(const std::string &pathname,
			    uint32_t mask,
			    functor_type &&functor)
{
	return do_create(pathname, mask,
			 watch_callback_t::create(std::forward<functor_type>
						  (functor)));
}

#if 0
{
#endif
}
#endif
