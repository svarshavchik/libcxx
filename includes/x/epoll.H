/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_epoll_H
#define x_epoll_H

#include <x/exceptionfwd.H>
#include <x/epollfwd.H>
#include <x/epollobj.H>
#include <x/fdfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	//! Event callbacks

	//! Derive from this object, implement event() and pass a reference
	//! to this object to
	//! \link fdObj::epoll_add(uint32_t,const epoll &,const ptr<epollCallbackObj> &) fd->epoll_add()\endlink.
	//! event() subsequently gets called to process the file descriptor
	//! events, until this callback object is no longer associated with
	//! the file descriptor and its events.
	//!
	//! This class should be referenced as
	//! \c epoll::callbackObj.

	class epollCallbackObj : virtual public obj {

	public:
		//! Constructor
		epollCallbackObj();

		//! Destructor
		~epollCallbackObj() noexcept;

		//! The object mask of possible events.

		//! Possible values are \c POLLIN and \c POLLOUT.

		typedef uint32_t event_t;

		//! A registered file descriptor event has occured
		virtual void event(//! The file descriptor with the event
				   const fd &fileDesc,

				   //! The event(s), a logical mask of \c EPOLLIN, \c EPOLLOUT, and others.
				   event_t events)=0;
	};

#if 0
};
#endif


#if 0
{
#endif
}
#endif
