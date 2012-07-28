/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_epollfwd_H
#define x_epollfwd_H

#include <x/ptrfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class epollObj;
class epollCallbackObj;

class epollBase;

//! Kernel \c %epoll(7) interface.

//! This is an interface to the kernel \c %epoll(7) API.
//!
//! \par Creating an %epoll set
//!
//! \code
//! epoll epollSet(epoll::create());
//! \endcode
//!
//! Instantiate an %epoll object, and invoke the
//! \c create() method to create an epoll set.
//!
//! \par Adding file descriptors to an %epoll set
//!
//! \code
//!
//! class myEpollCallbackObj : public epoll::callbackObj {
//!
//! public:
//!
//!    myEpollCallbackObj();
//!    ~myEpollCallbackObj();
//!    void event(const fd &fileDesc,
//!               event_t events);
//! };
//!
//! ptr<myEpollCallbackObj> cb(new myEpollCallbackObj);
//!
//! fd myFd=netaddr("mail.example.com", "smtp")->connect();
//!
//! myFd->epoll_add(EPOLLIN, epollSet, cb);
//! \endcode
//!
//! A callback object gets derived from epoll::callbackObj. Its event() method
//! gets invoked whenever the file descriptor registers an event.
//!
//! The file descriptor's \link fdObj::epoll_add() epoll_add() \endlink method adds the
//! file descriptor to the %epoll set, with the given event mask, and attaches
//! a \ref ptr "reference" to 
//! the given callback object.
//!
//! The event mask can be modified any time:
//!
//! \code
//! myFd->epoll_mod(EPOLLOUT);
//! \endcode
//!
//! This file descriptor's event mask gets modified accordingly. The callback
//! object remains attached to the file descriptor object.
//!
//! \par Adding or modifying multiple file descriptors.
//!
//! A convenience template function is provided to allow addition or
//! modification of multiple file descriptors, at once, represented as an
//! STL container:
//!
//! \code
//! std::list<fd> myFdList;
//!
//! //...
//!
//! epollSet->epoll_add(myFdList, EPOLLIN, cb);
//!
//! \endcode
//!
//! or:
//!
//! \code
//! epollSet->epoll_add(myFdList.begin(), myFdList.end(), EPOLLIN, cb);
//! \endcode
//!
//! \note
//! \link netaddrObj::bind() netaddr->bind() \endlink conveniently populates a
//! \c std::list<fd>.
//!
//! Also note that the same callback object gets attached to all file
//! descriptors.
//!
//! \par Waiting for events
//!
//! \code
//! epollSet->epoll_wait();
//! \endcode
//!
//! The \link epollObj::epoll_wait() epoll->epoll_wait() \endlink method
//! waits for events to occur for file descriptors in the event set, and
//! invokes the event() method for every file descriptor that registers an
//! event.
//!
//! \par Processing events
//!
//! \code
//!
//! void myCallbackObj::event(const fd &fileDesc,
//!                           event_t events)
//!
//! if (events & EPOLLIN)
//! {
//!    // ...
//! }
//! \endcode
//!
//! \c epoll_wait() invokes the event() method of a callback object attached
//! to each file descriptor that has
//! a registered event. A single callback object may be attached to multiple
//! file descriptors. The first argument to event() indicates which specific
//! file descriptor the event is for.
//!
//! \par Removing the file descriptor from the event set
//!
//! \code
//! myFd->epoll_mod();
//! \endcode
//!
//! Invoking \link fdObj::epoll_mod(uint32_t) fd->epoll_mod() \endlink with the default
//! 0 parameter removes the file descriptor from the event set, and
//! disassociates with the callback object. Another
//! alternative is to just close the file descriptor.
//!
//! \note
//! Each file descriptor saves an internal reference to its \c epoll set when
//! its added to one. A reference to the associated object is also saved.
//! Letting an epoll object go out of scope won't
//! automatically destroy the \c epoll set. Each file descriptor must be
//! explicitly removed from it, before the \c epoll set can be destroyed.
//! Closing a file descriptor removes it from the epoll set.

typedef ref<epollObj, epollBase> epoll;

//! A possibly unbound reference pointer to an epoll set.

//! \see epoll

typedef ptr<epollObj, epollBase> epollptr;

//! This is \c epoll::base and \c epollptr::base

class epollBase : public ptrrefBase {

public:

	//! A callback object

	//! This is an alias for the epoll callback object. This object
	//! should be referenced as \c epoll::callbackObj.

	typedef epollCallbackObj callbackObj;

	//! A reference to a callback object

	//! This is an alias for the epoll callback object. This object
	//! should be referenced as \c epoll::callback.

	typedef ptr<callbackObj> callback;
};

#if 0
{
#endif
}
#endif
