/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fdlistener_H
#define x_fdlistener_H

#include <x/fdlistenerobj.H>
#include <x/ptr.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A file descriptor listening thread

//! \code
//!
//! class myServerThread : virtual public INSERT_LIBX_NAMESPACE::obj {
//!
//! public:
//!     void run(const INSERT_LIBX_NAMESPACE::fd &socket, const INSERT_LIBX_NAMESPACE::fd &termfd)
//!     {
//!         // ...
//!     }
//! };
//!
//! INSERT_LIBX_NAMESPACE::ref<myServerThread> server(myServerThread::create());
//!
//! INSERT_LIBX_NAMESPACE::fdlistener listener(INSERT_LIBX_NAMESPACE::fdlistener::create(4000));
//!
//! listener->start(server);
//!
//! // ...
//!
//! listener->stop();
//! listener->wait();
//! \endcode
//!
//! This is a reference to a reference-counted object that implements a
//! thread which listens on one or more server sockets and accepts connections.
//! A thread gets started for each accepted connection.
//!
//! The \link fdlistenerObj::fdlistenerObj() create()\endlink function takes
//! one required parameter: a port specification.
//! The parameter may be specified as: an
//! \c int; a string that specifies the port by name or
//! number; a string containing a list port names or numbers separated by
//! commas or spaces, specifying each port by name or number; or an explicit
//! \c std::list&lt;int&gt;, specifying a list of port numbers.
//!
//! Additional parameters to create() fine tune the thread creation parameters.
//!
//! The constructor does not start a thread, but only creates the
//! listening ports (and throws an %exception if a port cannot be created,
//! possibly being in use already). start() creates the thread, which begins
//! listening. start() takes a reference to an object that implements
//! run() that takes two parameters.
//! The socket gets passed as a first argument to run(). The second parameter
//! to run() is a file descriptor that becomes readable when the listener's
//! stop() gets invoked. It should be set as a terminator file descriptor on
//! the socket, and if the server reads on the socket, that simulates an EOF,
//! and the server thread can terminate.
//!
//! All threads, all connections, that invoke run() do it on the same object
//! that was passed to start().
//!
//! stop() signals the listening thread to terminate, and makes the
//! terminator file descriptor readable. stop() does not wait
//! for the thread to terminate, invoke wait() to do that.
//!
//! \note
//! The second parameter to run() is the termination file
//! descriptor. stop() makes the termination file descriptor readable by
//! all threads, which should result in all threads that
//! are reading and writing from the connection getting a read and write
//! timeouts at the first possible opportunity, presumably terminating.
//! The listener thread waits for all started threads to terminate first, before
//! it terminates itself.

typedef ref<fdlistenerObj> fdlistener;

//! A pointer to a reference-counted file descriptor listening thread object.

//! \see fdlistener

typedef ptr<fdlistenerObj> fdlistenerptr;

#if 0
{
#endif
}
#endif
