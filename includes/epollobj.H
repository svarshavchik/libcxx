/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_epollobj_H
#define x_epollobj_H

#include <x/obj.H>
#include <x/ptrfwd.H>
#include <x/fdobj.H>
#include <x/sys/epollsys.h>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class epollObj;
class epollCallbackObj;
class epollBase;

//! \c %epoll(7) implementation.

//! This reference-counted object is not normally used directly, but through
//! an ::epoll reference handle. This object provides low-level wrappers for
//! \c %epoll(7) system calls.

class epollObj : public fdObj {

	//! How many file descriptors are in this %epoll set.

	//! This is tracked manually, in order to size the event array.
	//!
	refcnt fdcnt;

public:
	//! Default constructor.
	epollObj();

	friend class fdObj;
	friend class epollBase;

	//! Default destructor.
	~epollObj() noexcept;

private:
	//! epoll(2) file descriptor operation

	//! This is an interface to the epoll_ctl(2) system call.
	//! This method invokes the \c EPOLL_CTL_ADD, \c EPOLL_CTL_MOD, or
	//! \c EPOLL_CTL_MOD operation on the given file descriptor.

	void epoll_ctl(//! \c EPOLL_CTL_ADD, \c EPOLL_CTL_MOD, or \c EPOLL_CTL_DEL.

		       int op,
		       //! The file descriptor object.

		       fdObj *fileObj,

		       //! The event mask.

		       //! May contain the following flags: \c EPOLLIN,
		       //! \c EPOLLOUT, \c EPOLLHUP, \c EPOLLPRI, \c EPOLLET,
		       //! and \c EPOLLONESHOT. This is optional for the
		       //! \c EPOLL_CTL_DEL operation.

		       uint32_t eventmask=0) LIBCXX_HIDDEN;
public:
	//! Wait for epoll(2) events.

	//! This method waits for one or more epoll events to occur.
	//! \see ::epoll for more information.
	void epoll_wait(//! Number of milliseconds, -1 means no timeout.
			int timeout=-1);

	//! Add file descriptors to the epoll set.

	//! This template function adds a container of file descriptors or
	//! pollable objects to this epoll set. All file descriptor share the
	//! same callback object.

	template <typename T>
	void epoll_add(//! Any STL container that implements begin() and end().

		       T &fdEvents,

		       //! File descriptor %epoll events.

		       //! See
		       //! \link fdObj::epoll_add(uint32_t,const epoll &,const ptr<epollCallbackObj> &) fd->epoll_add()\endlink
		       //! for more information.

		       uint32_t newEvents,

		       //! Callback object

		       const ptr<epollCallbackObj> &callback)
	{
		epoll_add(fdEvents.begin(), fdEvents.end(), newEvents,
			  callback);
	}

	//! Add file descriptors to the epoll set.

	//! \overload
	//!
	//! This template function adds a set of file descriptors passed
	//! as a beginning and an ending iterator.

	template <typename T>
	void epoll_add(//! The beginning iterator.

		       T b,

		       //! The ending iterator.

		       T e,

		       //! File descriptor %epoll events.

		       //! See \link fdObj::epoll_add(uint32_t,const epoll &,const ptr<epollCallbackObj> &) fd->epoll() \endlink
		       //! for more information.

		       uint32_t newEvents,

		       //! Callback object

		       const ptr<epollCallbackObj> &callback)
	{
		epoll me(this);

		while (b != e)
		{
			(*b)->epoll_add(newEvents, me, callback);
			++b;
		}
	}
#include <x/sys/kqueuenb.h>
};

#if 0
{
#endif
}
#endif
