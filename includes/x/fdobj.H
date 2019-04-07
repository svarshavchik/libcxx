/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fdobj_H
#define x_fdobj_H

#include <x/obj.H>
#include <x/fdbaseobj.H>
#include <x/ptrfwd.H>
#include <x/basicattr.H>
#include <x/fdfwd.H>
#include <x/epollfwd.H>
#include <x/exceptionfwd.H>
#include <x/namespace.h>
#include <x/weakptr.H>
#include <x/sockaddrfwd.H>
#include <x/threads/timertaskfwd.H>
#include <string>
#include <vector>
#include <string_view>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <poll.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {
		class sessionObj;
	}

#if 0
};
#endif

class eventfdObj;
class timerfdObj;
class fdObj;

class fdbufferObj;
class fdinputiter;
class fdoutputiter;

class timespec;

//! Base class for fd

//! This base class for the fd reference defines additional methods for fd
//! objects, beyond the default methods provided by the ptr template.

class fdBase {

public:
	//! A bare-bones buffer for reading or writing from a file descriptor.

	//! \internal
	//! This object is not used
	//! directly, but rather indirectly via \ref inputiter "fd::inputiter"
	//! or \ref outputiter "fd::outputiter".

	typedef ptr<fdbufferObj> buffer;

	//! A bare-bones input iterator that reads from a \ref buffer "buffer".

	//! Use \c fd::inputiter to refer to this type.
	//!
	//! \code
	//! fdbase filedesc;
	//!
	//! std::string str;
	//!
	//! std::copy(fd::inputiter(filedesc), fd::inputiter(),
	//!           std::back_insert_iterator<std::string>(str));
	//! \endcode
	//!
	//! This example reads the entire contents of filedesc into a string,
	//! until EOF is read.

	typedef fdinputiter inputiter;

	//! A bare-bones output iterator that writes to a \ref buffer "buffer".

	//! Use \c fd::outputiter to refer to this type.
	//!
	//! \code
	//! fdbase filedesc;
	//!
	//! std::string str;
	//!
	//! fd::outputiter iter(filedesc);
	//!
	//! iter=std::copy(str.begin(), str.end(), iter);
	//!
	//! // ...
	//!
	//! iter.flush();
	//! \endcode
	//!
	//! The flush() method forces buffered output to be written to the
	//! file descriptor. The destructor does not flush unwritten buffered
	//! data to the file descriptor, automatically.

	typedef fdoutputiter outputiter;

	//! Open a file

	//! Contruct a new file descriptor object for an opened file.
	//!
	//! \return a new file descriptor reference.
	//!
	//! \note The opened file descriptor's closeon-exec flag is set
	//! automatically.

	static fd open(const std::string_view &filename,
		       int flags,
		       mode_t mode=0666);

	//! Open POSIX shared memory

	static fd shm_open(//! The name of the shared memory segment.
			   const std::string_view &filename,

			   //! File open flags.
			   int flags,

			   //! New file's permissions
			   mode_t mode=0666);

	//! Unlink POSIX shared memory segment
	static void shm_unlink(const std::string_view &filename);

	//! Create a temporary file.

	static fd tmpfile();

	//! Create a temporary file in a given directory.

	static fd tmpfile(//! Directory name
			  const std::string_view &tmpdir);

	//! %dup() another file descriptor

	//! The given file descriptor is %dup()ed. The new file descriptor
	//! is kept in this object.

	static fd dup(//! Another file descriptor.
		      int filedesc);

	//! %dup() another file descriptor

	//! \overload
	//!
	static fd dup(//! Another file descriptor.

		      //! This would be a reference to another instance of this
		      //! class, or one of its subclasses.
		      const fd &filedesc );

	//! Create a temporary directory for this userid, with given permissions

	//! First, attempts to use /tmp/libx.app.uid.gid.mode, creating if it
	//! does not exist. If it exists, it verifies that the process's
	//! effective userid and groupid match this directory. Otherwise,
	//! uses mkdtemp (to hell with it).

	static std::string mktempdir(mode_t mode);

	//! Create and bind a temporary unix filesystem domain socket

	//! This method appends a random suffix to the specified filename
	//! prefix, and creates a filesystem domain socket.
	//!
	//! To prevent pollution, every measure must be taken in order to
	//! remove the file: including blocking
	//! SIGINT/SIGQUIT/SIGTERM/SIGHUP/SIGSTOP, and removing the file
	//! when done.
	//!
	//! If the socket cannot be created after a 1000 attempts, an ENOSPC
	//! %exception gets thrown.

	static std::pair<fd, std::string>
	tmpunixfilesock(//! Prefix for the filesystem domain socket.

			//! Should be "/tmp/<something>".
			const std::string_view &pfix);

	//! Make fd::create() work as expected, without a template

	class newobj {

	public:

		//! Create a file under a temporary name, rename it after it's closed.

		//! This is \c INSERT_LIBX_NAMESPACE::fd::create().
		//! The new file gets created as "filename.tmp". After the
		//! resulting file is closed, the close() method renames
		//! "filename.tmp" to "filename".
		//!
		//! The file open flags are always \c O_CREAT|O_TRUNC|O_RDWR.

		static fd create(//! The final filename
				 const std::string_view &filename,

				 //! File creation mode
				 mode_t mode=0666);
	};

	//! Make fd::create() work as expected, without a template

	template<typename ptrrefType> class objfactory : public newobj {
	};

	//! Open a file first, then lockf() it.

	//! This opens a given file, then calls
	//! \ref fdObj::lockf(int,off64_t) "lockf(0, 0)", followed by a stat()
	//! to make sure that the filename's inode, after the lock,
	//! has not unexpectedly changed. When
	//! the file gets closed, or goes out of scope, the filename gets
	//! automatically unlink-ed.
	//!
	//! The inode check makes sure we didn't just lock a file that the
	//! previous owner has just unlink. If there's an inode mismatch, the
	//! stat() call is tried again, up to a ridiculously high number of
	//! attempts (after which point something must be wrong, so we
	//! throw an %exception).
	//!
	//! When the file gets closed, or goes out of scope, it's automatically
	//! unlink-ed.

	static fdptr lockf(//! The lock filename to create
			   const std::string_view &filename,

			   //! Locking mode

			   //! If set to F_TLOCK, and the file is locked,
			   //! a null file descriptor reference gets returned)
			   int lockmode=F_LOCK,

			   //! Permissions on the lock file
			   mode_t mode=0666);

	//! Create a pipe

	//! Use %pipe(2) to create a pipe.
	//! \return A pair. first=read side of the pipe, second=write
	//! side of the pipe.

	static std::pair<fd, fd> pipe();

	//! Create a socket pipe

	//! Use %socketpair(2) to create a socket pair pipe.
	//! \return A pair of file descriptors.

	//! file descriptor reference passed to this function gets the
	//! other end of the socket pipe.

	static std::pair<fd, fd> socketpair();

	//! Common code for pipe() and socketpair(), and several others.

	//! \internal
	//!
	static fd adopt(int filedesc) LIBCXX_HIDDEN;

	//! C++-friendly realpath()

	static std::string realpath(const std::string_view &pathname);

	//! Combine two paths.

	static std::string combinepath(const std::string_view &reference,
				       const std::string_view &combined);

	//! Common code for pipe() and socketpair()

	//! \internal
	//!
	static std::pair<fd, fd> pipesocket_common(int *pipefd)
		LIBCXX_INTERNAL;

	//! Listen on a socket.

	//! This is a wrapper for the listen(2) system call. This is a version
	//! that takes a list of fd objects that's returned by
	//! \link netaddrObj::bind netaddr->bind() \endlink.
	//!
	//! \note
	//! On Linux, listening on a wildcard IPv4 and a wildcard IPv6
	//! address fails with \c EADDRINUSE. This method detects this
	//! situation, and quietly removes the IPv4 fd from the list.

	static void listen(//! Created file descriptors are placed here.

			   std::list<fd> &fdList,

			   //! Default connection backlog for each file descriptor.

			   //! See the %listen(2) system call documentation
			   //! for more information.
			   int backlog=

#ifdef SO_MAXCONN
			   SO_MAXCONN
#else
			   20
#endif
			   );


	//! Create a socket

	//! Invokes fdObj::socket() for the referenced fdObj object.
	//! A new fdObj object gets instantiated if this fd reference is not
	//! bound to any fdObj object.
	//! Create a new socket

	static fd
	socket(//! \c PF_UNIX, \c PF_INET, or others.
	       int domain,

	       //! The socket type, \c SOCK_STREAM, \c SOCK_DGRAM, or others.
	       int type,
	       //! A domain-specific protocol (use 0 for the default one).

	       int protocol=0);

	//! Return the current working directory

	static std::string cwd();

	//! Return socket name of a native file descriptor

	static sockaddr getsockname(int filedesc);

	//! Return peer name of a native file descriptor

	static sockaddr getpeername(int filedesc);

#include <x/sys/credentialssys.h>

};

//! A wrapper for a file descriptor.

//! This class implements basic operations for a file descriptor.
//! An instance of the class takes ownership of a file descriptor,
//! after an open(), or some other function that creates a new file
//! descriptor, and provides wrappers for common file descriptor operations.
//! When an instance of this class is destroyed, if it still owns a
//! file descriptor, the file descriptor gets automatically closed.

class fdObj : virtual public obj,
	      public basic_attr,
	      public fdbaseObj {

private:
	//! Pointer to an epoll set that contains this open file descriptor.

	//! This pointer is unbound if this file descriptor is not a member
	//! of an epoll set.

	epollptr epoll_set;

	//! Reference to the attached callback object for registered event.

	//! This reference is unbound if this file descriptor is not a member
	//! of an epoll set.
	ptr<epollCallbackObj> epoll_callback;

	class closeactionObj;

	//! Something should be done to the file, as part of close()

	ptr<closeactionObj> closeaction;

	//! Reset closeaction to an empty string

	void clearcloseaction() LIBCXX_INTERNAL;

protected:
	//! The file descriptor, or -1 if one is not open.

	int filedesc;

	//! Whether to generate SIGPIPE on broken sockets

	//! If \c false, a write() on a broken socket throws an %exception.
	//! If \c true, a write() on a broken socket sends a \c SIGPIPE signal
	//! (traditional behavior).

	bool sigpipeFlag;

private:
	//! Initialize class members to default values

	void init() noexcept;

public:
	friend class eventfdObj;

	friend class epollObj;
	friend class gnutls::sessionObj;

	//! The destructor.

	//! If the object already has an open file descriptor, it gets
	//! automatically closed, by calling close().
	//! \note Any thrown %exception errors from close() get ignored.
	//! Invoke close() explicitly to have any errors reported.
	~fdObj();

	//! Adopt a file descriptor.

	//! The file descriptor is now owned by this object.
	//! The file descriptor gets closed automatically if this
	//! object is destroyed without an explicit close().

	fdObj(//! Another file descriptor.

	      //! This file descriptor becomes owned by this object.
	      //!
	      int fd);


	//! Rename this file when it gets closed.

	//! This is used by \ref fdBase::newobj::create "fd::create()".
	//!
	void renameonclose(//! The filename.tmp file that was created.

			   const std::string_view &tmpname,

			   //! Rename to filename, upon close.

			   const std::string_view &filename);

	//! Unlink this file when it gets closed.

	//! This is used by \ref fdBase::lockf "fd::base::lockf()".

	void unlinkonclose(const std::string_view &filename);

	//! Cancel the newly-created file

	//! If the file was opened by the create() function, remove it.
	//! Do not have the following close() method rename the file.
	//! \note cancel() does not close the file. This file object must still
	//! be closed

	void cancel();

	//! Update the file's timestamp

	void futimens(const timespec &atime,
		      const timespec &mtime);

	//! Update the file's timestamp

	//! \overload
	//!

	void futimens(const timespec &atime);

	//! Update the file's timestamp to current time.

	//! \overload
	void futimens();

	//! Update the file's timestamp periodically

	//! \return a mcguffin. The file's timestamp gets periodically updated
	//! as long as the mcguffin exists. Internally, the file descriptor gets
	//! duped, and a separate thread invokes futimens() periodically, so
	//! even if this file is closed, its timestamp continues to get updated
	//! as long as the mcguffin remains in scope!

	template<typename duration_type>
	ref<obj> futimens_interval(const duration_type &interval);

private:
	//! Internal implementation function for futimens_interval()

	//! \internal

	std::pair<timertask, ref<obj> >	get_futimens_task();

	class futimens_intervalObj;
public:

	//! Make a socket connection.

	//! \overload
	//! The server's address is specified as an opaque chunk of memory.
	//! This is usually the \c addr parameter from an instance of
	//! netaddr::resultObj::addrstruct.

	void connect(//! The server's address.

		     const const_sockaddr &addr);

	//! Make a socket connection.

	//! connect() makes a client socket connection to a server, and
	//! is a wrapper for the connect(2) system call.

	void connect(//! The server's address.

		     const struct ::sockaddr *serv_addr,

		     //! serv_addr's length, in bytes

		     socklen_t addrlen);

	//! Set the \c SO_REUSEADDR socket %option.

	//! This is a wrapper for the setsockopt(2) system call that sets
	//! or clears the SO_REUSEADDR socket %option.

	void reuseaddr(//! Set or clear the flag
		       bool flag);

	//! Return the socket's address.

	//! This method returns the socket's bound local address.
	//! \code
	//! sockaddr addr(fd->getsockname());
	//! \endcode
	//! \return A \c sockaddr reference, to a
	//! \link sockaddrObj socket address \endlink object.

	sockaddr getsockname() const;

	//! Return the peer's address.

	//! This method returns the socket's peer's address.
	//! \code
	//! sockaddr addr(fd->getpeername());
	//! \endcode
	//! \return A \c sockaddr reference, to a
	//! \link sockaddrObj socket address \endlink object.

	sockaddr getpeername() const;

	//! Return peer's address, as a string

	//! This is same as getpeername(), except that if this is an anonymous
	//! local socket, return getsockname().

	std::string getpeername_str() const;

	//! Bind a socket to an address.

	//! This is a wrapper for the bind(2) system call.
	//! The address is specified as an opaque chunk of memory.
	//! This is usually the \c addr parameter from an instance of
	//! netaddr::resultObj::addrstruct.

	void bind(//! Socket's address.
		  const struct ::sockaddr *my_addr,
		  //! Length of the socket's address.
		  socklen_t my_len)
;

	//! Listen on a socket.

	//! This is a wrapper for the listen(2) system call.
	//!

	void listen(int backlog=

#ifdef SO_MAXCONN
		    SO_MAXCONN
#else
		    20
#endif
		    );

	//! Accept a connection.

	//! This is a wrapper for the accept(2) system call.
	//! \return the new connection. An %exception gets thrown if the
	//! accept(2) system call fails, unless \c errno is \c EAGAIN or
	//! \c EWOULDBLOCK. This indicates no pending connections on a
	//! non-blocking socket. In this case, accept() returns a null ptr.
	//!
	//! The new socket has the close-on-exec flag set. Other socket
	//! flags are cleared (notably, the nonblocking flag, inherited
	//! by the new socket from the parent, on Linux) is cleared.
	fdptr accept(//! Peer's address.

		     sockaddrptr &peername);

	//! Accept a connection.

	//! \overload
	//!
	fdptr accept();

	//! Bind a socket to an address.

	//! \overload
	void bind(//! The server's address.

		  const const_sockaddr &addr);

	//! Close the file descriptor.

	//! An %exception gets thrown if open fails. close() does nothing
	//! if this file descriptor is not open.

	void close();

	//! Shut down part of a full duplex connection

	void shutdown( //! \c SHUT_RD, \c SHUT_WR, or \c SHUT_RDWR.
		      int how);

	//! Read from the file descriptor.

	//! \return number of bytes read from the file descriptor.
	//!
	//! \note
	//! A read error also return 0, and sets errno.
	//!
	//! An end-of-file condition gets
	//! reported with a 0 return, and with \c errno set to 0.

	inline size_t read(//! Buffer
			   char *buffer,

			   //! Element count
			   size_t cnt)

	{
	again:
		ssize_t n=::read(filedesc, buffer, cnt);

		if (n < 0)
		{
			if (errno == ECONNRESET)
			{
				errno=0;
				return 0;
			}

			if (errno == EINTR)
				goto again;

			return 0;
		}

		errno=0;
		return n;
	}

#include <x/sys/prw64.h>

	using fdbaseObj::write;

	//! Write to the file descriptor.

	//! \return number of bytes written.
	//! Errors are reported by returning 0.

	inline size_t write(//! Buffer
			    const char *buffer,

			    //! Element count
			    size_t cnt)
	{
		errno=0;

		if (cnt == 0)
			return 0;

	again:
		ssize_t n=sigpipeFlag ? ::write(filedesc, buffer, cnt)
			: ::sendto(filedesc, buffer, cnt, MSG_NOSIGNAL,
				   0, 0);

		if (n == 0)
			errno=ENOSPC;

		if (n < 0)
		{
			if (errno == EINTR)
				goto again;

			n=0;
		}
		return n;
	}

	//! Write to the file descriptor.

	//! This method uses the send() system call instead of write().
	//! \return number of elements written, or 0 if there was a write error.,

	inline size_t send(//! Buffer

			   const char *buffer,

			   //! Element count

			   size_t cnt,

			   //! message flags

			   int flags,

			   //! Destination address

			   const struct ::sockaddr *to_addr=0,

			   //! Size of the destination address

			   socklen_t to_len=0)
	{
	again:
		ssize_t n=::sendto(filedesc, buffer, cnt, flags, to_addr,
				   to_len);
		if (n < 0)
		{
			if (errno == EINTR)
				goto again;


			errno=0;
		}

		return n;
	}

	//! Send file descriptors over a filesystem domain socket to another process

	//! This file descriptor must be a connected filesystem domain socket.
	//! \return \c false if the underlying file descriptor is non-blocking
	//! and this operation would block, \c true if the file descriptors
	//! were sent, and a thrown %exception for all other error conditions.

	bool send_fd(//! Array of file descriptors to send

		     const int *fdesc_array,

		     //! How many file descriptors there are

		     size_t fdesc_array_size);

	//! Send file descriptors over a filesystem domain socket to another process

	//! \overload
	//!
	bool send_fd(//! Array of file descriptors to send
		     fd *fdesc_array,

		     //! How many file descriptors there are

		     size_t fdesc_array_size);

	//! Send file descriptors over a filesystem domain socket to another process

	//! \overload
	//!
	bool send_fd(//! Array of file descriptors to send
		     fdptr *fdesc_array,

		     //! How many file descriptors there are

		     size_t fdesc_array_size);

	//! Send file descriptors over a filesystem domain socket to another process

	//! \overload
	//!
	bool send_fd(//! Array of file descriptors to send
		     const std::vector<fd > &fdesc_array)
;


	//! Send file descriptors over a filesystem domain socket to another process

	//! \overload
	//!
	bool send_fd(//! Array of file descriptors to send
		     const std::vector<int> &fdesc_array)
;


	//! Send a file descriptor over a filesystem domain socket

	bool send_fd(//! The file descriptor to send
		     const fd &fDesc);

	//! Send a file descriptor over a filesystem domain socket

	//! \overload
	//!
	bool send_fd(//! The file descriptor to send
		     int fDesc);


	//! Receive file descriptors from a filesystem domain socket

	//! \return How many file descriptor were received, which may be less
	//! then fdesc_array_size. A 0 gets returned if this file descriptor
	//! is non-blocking, and is empty. All other errors throw an exception.

	size_t recv_fd(//! File descriptors are placed here.
		       int *fdesc_array,

		       //! Size of the fdesc_array.

		       //! If more file descriptors were sent, the extra ones
		       //! are lost.
		       size_t fdesc_array_size);


	//! Receive file descriptors from a filesystem domain socket

	//! \overload

	size_t recv_fd(//! File descriptors are placed here.

		       //! Any open file descriptors in this array get closed
		       //! automatically.
		       //! \note Any thrown %exception errors from closing any
		       //! open file descriptors get ignored.
		       //! Close existing file descriptors explicitly to have
		       //! any errors reported.
		       //! \note any null file descriptor references are
		       //! initialized to refer to new file descriptor objects.
		       fdptr *fdesc_array,

		       //! Size of the fdesc_array

		       //! If more file descriptors were sent, the extra ones
		       //! are lost.
		       size_t fdesc_array_size);

	//! Receive file descriptors from a filesystem domain socket

	//! \overload
	//!

	void recv_fd(//! File descriptors are placed here.

		     //! Any open file descriptors in this array get closed
		     //! automatically.
		     //! \note This array must be resized beforehand, to
		     //! indicate the maximum number of file descriptors to
		     //! receive. When this function returns, the array size
		     //! gets adjusted to indicate how many file descriptors
		     //! were actually received.
		     //!
		     //! \note Any thrown %exception errors from closing any
		     //! open file descriptors get ignored.
		     //! Close existing file descriptors explicitly to have any
		     //! errors reported.
		     //!
		     //! \note Any null file descriptor references are
		     //! initialized to refer to new file descriptor objects.
		     std::vector<fdptr> &fdesc_array);

	//! Receive file descriptors from a filesystem domain socket

	//! \overload
	//!

	void recv_fd(//! File descriptors are placed here

		     //! The existing contents of this %vector are ignored,
		     //! except that
		     //! this array must be resized beforehand, to
		     //! indicate the maximum number of file descriptors to
		     //! receive. When this function returns, the array size
		     //! gets adjusted to indicate how many file descriptors
		     //! were actually received.
		     std::vector<int> &fdesc_array);

	//! Send my credentials to a filesystem domain socket

	//! \return \c false if the underlying file descriptor is non-blocking
	//! and this operation would block, \c true if the credentials
	//! were sent, and a thrown %exception for all other error conditions.

	bool send_credentials(//! Sending process ID
			      pid_t p=getpid(),

			      //! Sending user ID
			      uid_t u=getuid(),

			      //! Sending group ID

			      gid_t g=getgid());

	//! Enable this filesystem domain socket to receive credentials

	void recv_credentials(bool flag);

	//! Receive another process's credentials

	//! \return \c false if the underlying file descriptor is non-blocking
	//! and this operation would block, \c true if the credentials
	//! were received, and a thrown %exception for all other error
	//!  conditions.

	bool recv_credentials(fd::base::credentials_t &uc)
;

	//! Seek to a new position for the file descriptor.

	//! Errors are reported by throwing an %exception.
	//!
	off64_t seek(//! Offset.

		     off64_t offset,

		     //! Either \c SEEK_SET, \c SEEK_CUR, or \c SEEK_END.

		     int whence);

	//! Truncate the file
	void truncate(//! New file size
		      off64_t offset);

	//! Set or clear the close-on-exec flag.

	//! The file descriptor's close-on-exec flag gets
	//! set accordingly.
	void closeonexec(//! If \c true, the flag is set.
			 bool flag);

	//! Retrieve the current value of the closeonexec flag

	bool closeonexec();

	//! Set or clear the IPv6 only flag.

	//! For IPv6 sockets, this flag controls whether the socket can reach
	//! IPv6-mapped IPv6 addresses.

	void ipv6only(//! If \c true, the socket can only connect with other IPv6 addresses
		      bool flag);

	//! Retrieve the current value of the ipv6only flag
	bool ipv6only();

	//! Set or clear the sigpipe flag.

	//! This setting determines if writing to a broken socket throw
	//! an %exception or send a signal.
	//!
	//! This is enabled by default, for sockets.

	void sigpipe(//! If \c true, writes on a broken write send \c SIGPIPE

		     //! The default \c false setting throws an %exception
		     bool flag) noexcept
	{
		sigpipeFlag=flag;
	}

	//! Set or clear the non-blocking mode.

	//! The file descriptor's non-blocking mode is enabled or disabled
	//! accoringly.
	virtual void nonblock(//! If \c true, the non-blocking mode is set.
			      bool flag);

	//! Get the file object's non-blocking mode.

	//! \return \c true if the file descriptor is in the non-blocking
	//! mode.

	virtual bool nonblock();

	//! Set or clear keepalive messages.

	//! The socket's keepalive messages are enabled or disabled.
	//!
	void keepalive(//! \c true, to enable keepalive
		       bool flag);

	//! Get the socket's keepalive message flag

	bool keepalive();

private:
	//! Set the sigpipe flag based on the file descriptor type

	//! \internal

	void setsigpipe(int fd) noexcept LIBCXX_HIDDEN ;

private:

	//! Retrieve standard file status attributes.

	//! \internal
	//! \see basic_attr::stat_internal().

	int stat_internal(//! The buffer where file status is placed, on success.
			  struct ::stat *buf) noexcept override LIBCXX_HIDDEN;

	//! List this file descriptor's extended attributes.

	//! \internal
	//! \see basic_attr::listattr_internal().
	ssize_t listattr_internal(const listattr_internal_args &args)
		noexcept override LIBCXX_HIDDEN;

	//! Return the value of an attribute

	//! \internal
	//! \see basic_attr::getattr_internal().
	ssize_t getattr_internal(//! Attribute's name.
				 const char *name,

				 //! Buffer for the attribute's value.
				 void *value,

				 //! Size of the value buffer.
				 size_t size)
		noexcept override LIBCXX_HIDDEN;

	//! Set the attribute's value.

	//! \internal
	//! \see basic_attr::setattr_internal().

	int setattr_internal(//! Attribute's name.
			     const char *name,
			     //! Attribute's value.
			     const void *value,
			     //! Size of the attribute's value.
			     size_t size,
			     //! Optional flags: \c XATTR_CREATE or \c XATTR_REPLACE.
			     int flags)
		noexcept override LIBCXX_HIDDEN;

	//! Remove an attribute.

	//! \internal
	//! \see basic_attr::removeattr_internal().
	int removeattr_internal(//! Attribute's name
				const char *name)
		noexcept override LIBCXX_HIDDEN;

	//! Set the file permission bits

	//! \internal
	//! \see basic_attr::chmod_internal().
	int chmod_internal(mode_t mode) noexcept override LIBCXX_HIDDEN;

	//! Set the file ownership and permissions

	//! \internal
	//! \see basic_attr::chown_internal().
	int chown_internal(uid_t uid, gid_t group) noexcept override
		LIBCXX_HIDDEN;

	//! I am a file descriptor.

	//! \internal
	std::string whoami() override LIBCXX_HIDDEN;

public:

	//! An interface to the lockf() system call.

	//! This function performs a POSIX lock operation on the file
	//! descriptor. See the lockf(3) man page for more information.
	//! \return \c false, if \c F_TLOCK or \c F_TEST fails with
	//! \c EACCES or \c EAGAIN, or \c EDEADLK, \c true otherwise.
	//! All other errors throw an %exception.

	bool lockf(//! \c F_LOCK, \c F_TLOCK, \c F_ULOCK, or  \c F_TEST.

		   int cmd,

		   //! Byte count for the file lock operation.

		   off64_t len=0);


	//! Add this file descriptor to an epoll set.

	//! This file descriptor is added to the given epoll set. If this
	//! file descriptor was previously added to some other epoll set,
	//! it gets removed from that epoll set, fist. Although the kernel
	//! %epoll interface allows adding the same file descriptor to
	//! multiple %epoll sets, this implementation limits a file descriptor
	//! to one %epoll set.
	//!
	//! \see ::epoll for more information.
	void epoll_add(
		       //! File descriptor %epoll events. See epoll_ctl(7).

		       //! This is a bitwise mask of the following values:
		       //! \c EPOLLIN,
		       //! \c EPOLLOUT,
		       //! \c EPOLLRDHUP,
		       //! \c EPOLLPRI,
		       //! \c EPOLLET, and
		       //! \c EPOLLONESHOT.
		       uint32_t newEvents,

		       //! Add this file descriptor to this %epoll set.

		       const epoll &epollSet,

		       //! Invoke the following callback when the event occurs.

		       //! The referenced callback object gets associated with
		       //! this file descriptor's events.

		       const ptr<epollCallbackObj> &epollCallbackArg
		       );

	//! Modify this file descriptor's event mask.

	//! This file descriptor must have been previously added to an
	//! epoll set. This file descriptor's event mask gets modified
	//! accordingly.
	//!
	//! \see ::epoll for more information.
	void epoll_mod(
		       //! New file descriptor %epoll events.

		       //! If the new event mask is 0, this file descriptor
		       //! gets removed from the %epoll set (an
		       //! \c EPOLL_CTL_DEL operation), and the callback object
		       //! ges detached.
		       //!
		       //! A subsequent call to this epoll() function throws
		       //! an %exception. The file descriptor must be explicitly
		       //! added to an epoll set by invoking
		       //! epoll(uint32_t,ptr<epollObj>);
		       uint32_t newEvents=0);

	//! Return the file descriptor, inherited from fdbaseObj

	//! \return the currently open file descriptor, or -1.
	//!
	int get_fd() const noexcept override;

private:
	//! Implement pubread(), inherited from fdbaseObj

	size_t pubread(//! Buffer

		       char *buffer,

		       //! Element count

		       size_t cnt) override LIBCXX_HIDDEN;

	//! Implement pubpoll(), inherited from fdbaseObj
	int pubpoll(//! Timeout
		    int timeout_ms) override LIBCXX_HIDDEN;

	//! Implement pubread_pending(), inherited from fdbaseObj

	size_t pubread_pending() const override LIBCXX_HIDDEN;

	//! Implement pubwrite(), inherited from fdbaseObj

	size_t pubwrite(//! Buffer

			const char *buffer,

			//! Element count

			size_t cnt) override LIBCXX_HIDDEN;

	//! Implement pubseek(), inherited from fdbaseObj

	off64_t pubseek(//! Offset.

			off64_t offset,

			//! Either \c SEEK_SET, \c SEEK_CUR, or \c SEEK_END.

			int whence) override LIBCXX_HIDDEN;

	//! Implement pubconnect(), inherited from fdbaseObj

	void pubconnect(//! The server's address.

			const struct ::sockaddr *serv_addr,

			//! serv_addr's length, in bytes

			socklen_t addrlen) override LIBCXX_HIDDEN;

	//! Implement pubaccept(), inherited from fdbaseObj

	fdptr pubaccept(sockaddrptr &peername) override LIBCXX_HIDDEN;

	//! Implement pubclose(), inherited from fdbaseObj

	void pubclose() override LIBCXX_HIDDEN;

	template<bool serialize_flag>
	class do_serialize;

public:
	template<bool serialize_flag>
	friend class do_serialize;

	//! Serialization function

	//! In the contents of a file descriptor, serializing involves
	//! reading the contents of the file and putting it into the
	//! the serialization stream. Deserializing writes out the
	//! file from the contents of the serialization stream.
	//!
	//! The serialized file contents consists of consecutive blocks
	//! of char sequences, ending with an empty block.
	//!
	//! The file descriptor must be positioned at the beginning of the
	//! file before it gets serialized or deserialized.
	//!
	//! If the file descriptor is not open, empty contents get serialized
	//! (essentially an empty file). A deserialization of a closed file
	//! descriptor ends up discarding the serialized data.

	template<typename iter_type>
	void serialize(iter_type &i)
	{
		do_serialize<iter_type::serializing>::serialize(*this, i);
	}

private:
	//! Implement file descriptor contents' serialization

	template<typename iter_type>
	void impl_serialize(iter_type &i)
	{
		std::vector<char> buffer;
		size_t n;

		buffer.resize(get_buffer_size());

		do
		{
			n=filedesc < 0 ? 0:read(&buffer[0], buffer.size());

			i.range(buffer.begin(), buffer.begin()+n);
		} while (n > 0);
	}

	//! Implement file descriptor contents' deserialization

	template<typename iter_type>
	void impl_deserialize(iter_type &i)
	{
#if 0
		if (filedesc < 0)
			tmpfile();
#endif
		std::vector<char> buffer;

		do
		{
			buffer.clear();
			i(buffer);

			size_t buffer_s=buffer.size();

			if (buffer_s)
			{
				const char *buffer_p=&buffer[0];

				while (buffer_s)
				{
					size_t n=(filedesc < 0)
						? buffer_s
						:write(buffer_p, buffer_s);

					buffer_p += n;
					buffer_s -= n;
				}
			}

		} while(buffer.size() > 0);
	}

	class recvmsg_helper;
};

//! Helper class for file descriptor serialization

template<>
class fdObj::do_serialize<true> {

public:

	//! Implement file descriptor contents' serialization

	template<typename iter_type>
	static void serialize(fdObj &fd,
			      iter_type &i)
	{
		fd.impl_serialize(i);
	}
};

//! Helper class for file descriptor deserialization

template<>
class fdObj::do_serialize<false> {

public:

	//! Implement file descriptor contents' deserialization

	template<typename iter_type>
	static void serialize(fdObj &fd,
			      iter_type &i)
	{
		fd.impl_deserialize(i);
	}
};

#if 0
{
#endif
}
#endif
