/*
** Copyright 2015-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_inotifyobj_H
#define x_inotifyobj_H

#include <x/fdobj.H>
#include <x/mpobj.H>
#include <x/weakmultimapfwd.H>
#include <x/ptrfwd.H>

#include <map>
#include <functional>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A subclass of fdobj that implements inotify(7)

class LIBCXX_PUBLIC inotifyObj : public fdObj {

public:
	//! Functor callback reference

	typedef std::function<void(uint32_t, uint32_t,
				   const char *)> watch_callback_t;
private:

#pragma GCC visibility push(hidden)

	class watcherObj;

	//! A weak map of all active watchers

	typedef weakmultimap<int, watcherObj> watchers_t;

	//! Active watchers

	watchers_t watchers;

#pragma GCC visibility pop
public:
	//! The constructor

	//! The constructor creates a inotify file descriptor

	inotifyObj();

	//! Destructor
	~inotifyObj();

	class watchObj;

	//! Adds a new watcher

	ref<obj> create(//! Pathname to watch
			const std::string &pathname,

			//! Flags
			uint32_t mask,

			//! Functor to invoke

			const watch_callback_t &functor);

	//! Process the next inotify event
	void read();
};

#if 0
{
#endif
}
#endif
