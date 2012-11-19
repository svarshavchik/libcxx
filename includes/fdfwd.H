/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fdfwd_H
#define x_fdfwd_H

#include <x/ptrfwd.H>
#include <x/serializationfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class fdObj;
class fdBase;

//! A file descriptor object.

//! \ref fdptr "fdptr" is a \ref ptr "reference-counted" pointer to a file
//! descriptor object. \ref fd "fd" is a non-null
//! \ref ref "reference" to a file descriptor object. The file descriptor
//! object, \ref fdObj "fdObj", which implements most traditional file
//! operations. When the last reference goes out of scope and
//! \ref fdObj "fdObj" gets destroyed, the file descriptor gets closed
//! automatically.
//!
//! \par Creating an uninitialized file descriptor object
//!
//! \code
//! fdptr myFd(fdptr::create());
//! \endcode
//!
//! This creates a reference to a new file descriptor object, that has not
//! opened anything, yet.
//!
//! \see fdObj
//!
//! \par Opening files
//!
//! \code
//! fdptr myFd;
//!
//! myFd=fdptr::base::open("/etc/passwd", O_RDONLY);
//! \endcode
//!
//! This creates a reference to a file descriptor object for an opened file.
//!
//! \code
//! fdptr myFd(fdptr::base::tmpfile());
//! \endcode
//!
//! Thie new file descriptor object references a newly-created temporary file.
//!
//! \see fdObj::tmpfile()
//!
//! \par Creating files under a temporary name
//! \code
//! fdptr myFd;
//!
//! myFd=fdptr::create("filename", 0600);
//! \endcode
//!
//! The create() function enables creating a file under a temporary name.
//! The subsequent close() function automatically renames the file to its
//! original name, after it's closed.
//!
//! The file gets actually created as "filename.tmp". create() always uses the
//! file open flags of \c O_CREAT|O_TRUNC|O_RDWR.
//!
//! \par Creating a pipe
//!
//! \code
//! std::pair<fd, fd> fdpipe(fd::base::pipe());
//! \endcode
//!
//! Use %pipe(2) to create a pipe. \c fdpipe.first is the read side of the
//! pipe. \c fdpipe.second is the write side of the pipe.
//!
//! \code
//! std::pair<fd, fd> fdpair(fd::base::socketpair());
//! \endcode
//!
//! Use %socketpair(2) to create a socket pair pipe.
//! Both ends of the socket pipe, \c first and \c second,
//! read/write with the other end.
//!
//! \par Creating a single socket
//!
//! \code
//! fd myFd(fd::base::socket(PF_INET, SOCK_STREAM, 0);
//! \endcode
//!
//! Use %socket(2) to create a new socket.
//!
//! \par Listening on sockets
//!
//! \code
//! std::list<fd> listen_fds;
//!
//! // ...
//!
//! fd::base::listen(listen_fds, 20);
//! \endcode
//!
//! fd::listen() takes a list of file descriptors that's initialized by
//! \link netaddrObj::bind netaddr->bind()\endlink, and invokes the %listen(2)
//! system call on each one of them. The second parameter, the backlog, is
//! optional and defaults to a system-specific value.
//!
//! \note
//! Linux has \c IPV6_V6ONLY disabled by default, so
//! listening on a wildcard IPv4 and a wildcard IPv6
//! address fails with \c EADDRINUSE. fd::base::listen() detects this
//! situation, and quietly removes the IPv4 file descriptor from the list.
//!
//! \par All other operations
//!
//! Once created, the methods of the underlying fdObj object implement the
//! usual file descriptor operations:
//!
//! \code
//! myFd->read(buffer, size);
//!
//! /* ... */
//!
//! myFd->seekpos(0);
//!
//! \endcode
//!
//! Since fd is an object reference, instances of fd may be freely passed
//! around, without affecting the underlying file object.
//!
//! \par Passing file descriptors
//!
//! \code
//! fd fdFile=fd::base::open(filename, O_RDONLY);
//!
//! fdSocketFd->send_fd(&fdFile, 1);
//! \endcode
//!
//! Special support exists for passing file descriptors across filesystem
//! domain sockets, see \c unix(7).
//!
//! Several overloaded send_fd() functions are provided for passing one or
//! more file descriptor to a process that hold the other end of the opened
//! socket. The receiver calls recv_fd() to receive those opened files.
//!
//! \par Close on exec
//!
//! Unlike the C library functions, file descriptors created by this class
//! have their close-on-exec flag automatically set upon creation. File
//! descriptors created by these object references do not, by default, get
//! inherited by exec-ed processes. The close-on-exec flag should be turned
//! off explicitly:
//!
//! \code
//!    fdRef->closeonexec(false);
//! \endcode
//! \see fdObj
//!
//! \par Implementation of the file descriptor transport interface
//!
//! The file descriptor object inherits from the
//! \ref fdbase "fdbase" interface, which defines several virtual methods:
//! pubread(), pubwrite(), pubseek(), pubconnect(), and several others.
//!
//! There are several subclasses of \c fdbase that take a file descriptor
//! transport interface and add additional functionality:
//!
//! - \ref fdtimeout "fdtimeout" - implement timeouts for reading or writing,
//! or connecting a socket.
//!
//! - \ref fdreadlimit "fdreadlimit" - implement a limit on the number of
//! bytes read from the underlying file descriptor.
//!
//! Each one of these subclasses takes a reference to a file descriptor
//! object, then exports the same \c fdbase interface.
//!
//! \par Current working directory
//!
//! \ref fdBase::cwd "fd::cwd"() returns the current working
//! directory.
//!
//! \par Updating the file timestamp
//!
//! When an opened file serves as a lock file, sometimes its useful to make
//! sure that the file's timestamp is current, when an old timestamp is a factor
//! in determining that the process that owns the lock file no longer exists.
//!
//! \ref fdObj::futimens "fd->futimens()" updates the opened file's timestamp,
//! to the current time, by default.
//! There's a mechanism for updating an opened file descriptor's timestamp
//! periodically, and automatically. This is done by
//! \ref fdObj::futimens_interval "fd->futimens_interval()". This method
//! starts a background
//! \ref timer "timer thread" that updates the opened file descriptor's
//! access and modification timestamp, at regular intervals.
//! The timer thread is shared by all file descriptors that are marked for
//! regular timestamp updates.
//!
//! fdObj::futimens_interval() returns a mcguffin. The file's timestamp
//! continues to get updated, at specified intervals, as long as the mcguffin
//! remains in scope. When the mcguffin goes out of scope and gets destroyed,
//! updates to the file's timestamp are stopped, and if there are no other
//! open file descriptors that have regularly-scheduled timestamp updates, the
//! timer thread stops (it gets resumed automatically if another file descriptor
//! is set up for regular timestamp updates, later).
//!
//! Internally, the file descriptor gets duplicated, and the duplicate file
//! descriptor gets used to update the file's timestamp. This means that even
//! if the file gets closed, its timestamp continues to get updated, as long
//! as the mcguffin remains in scope!

typedef ptr<fdObj, fdBase> fdptr;

//! A non-null reference to a file descriptor object.

//! \see fdptr
typedef ref<fdObj, fdBase> fd;

//! A nullable pointer reference to a constant file descriptor object.

//! \see fdptr

typedef const_ptr<fdObj, fdBase> const_fdptr;

//! A reference to a constant file descriptor object.

//! \see fdptr
typedef const_ref<fdObj, fdBase> const_fd;

namespace serialization {

	//! Specialization for deserializing file descriptors.

	template<>
	void default_traits::classcreate<fdptr>(fdptr &ptr);
}

#if 0
{
#endif
}
#endif
