/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fdtimeoutobj_H
#define x_fdtimeoutobj_H

#include <x/obj.H>
#include <x/ptr.H>
#include <x/fdbaseobj.H>
#include <x/exception.H>
#include <x/timespec.H>
#include <iostream>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Implement timeouts on a file descriptor.

//! This reference-counted class takes a fdbaseObj, and implements
//! another fdbaseObj that returns 0 bytes read or written
//! if the read or the write
//! operation on a file descriptor times out.
//!
//! Note that when 0 bytes read is returned, \c errno can be used to
//! discriminate between timeouts (errno will be ETIMEDOUT) and an ordinary
//! end of file operation (errno will bne 0)
//!
//! The read and write timeouts can be individually turned on or off.
//! When the timeout is off, this object simply passes through the read/write
//! request to the real file descriptor.
//! other effects.
//!
//! Only one timeout object may be attached to a file descriptor.
//!
//! \par Setting read and write timeouts
//!
//! \code
//!
//! INSERT_LIBX_NAMESPACE::fd socket;
//!
//! socket->nonblock(true);
//!
//! // ...
//!
//! INSERT_LIBX_NAMESPACE::fdtimeout socket_timeout(INSERT_LIBX_NAMESPACE::fdtimeout::create(socket));
//!
//! INSERT_LIBX_NAMESPACE::istream i(socket_timeout->getistream());
//!
//! socket_timeout->set_read_timeout(10);
//!
//! // ...
//!
//! std::getline(line, *i);
//!
//! if (i->fail())
//!     // Timed out
//!
//! \endcode
//!
//! In this example, if the timer expires before the line is read from the
//! socket, the internal pubread() function returns 0, aborting getline().
//!
//! set_read_timeout() sets a timer. The second parameter is actually a
//! \ref timespecAnchor "timespec". The timer, once set, remains in place until
//! it is cancelled, or goes off. If pubread()
//! completes before the timer goes off, the next call to pubread() will still
//! timeout and return 0 when the timer expires (or if it is
//! already expired).
//!
//! cancel_read_timer() cancels the read timer.
//!
//! set_write_timeout() and cancel_write_timer() implements an analogous timeout
//! for write(). The read and write timers are separate, but only one of each
//! is in effect at the same time. Invoking set_read_timeout()
//! or set_write_timeout() replaces the previous timer.
//!
//! In most cases, the file descriptor should be set to non-blocking mode in
//! order to have proper timeout semantics. When a read or a write timer is
//! in effect, reading or writing still blocks, on a non-blocking file
//! descriptor, until the file descriptor is ready for reading or writing,
//! or until the respective timer expires.
//!
//! A read or a write timeout results in a 0-byte read/write result with
//! an \c ETIMEDOUT errno.
//!
//! \par Periodic read and write timers
//!
//! \code
//! socket_timeout->set_read_timeout(8192, 10);
//! \endcode
//!
//! This example sets a ten second timeout (the second parameter is actually
//! a \ref timespecAnchor "timespec"), however if 8192 bytes are read succesfully
//! before the timer expires, it is automatically reset for another 8192
//! bytes. The timer keeps getting reset as long as the requisite number of
//! bytes are always read before it expires, and until it is explicitly
//! cancelled by cancel_read_timer().
//!
//! set_write_timeout() has a similar periodic functionality.
//!
//! \note
//! The timer keeps ticking in between successive read and write calls.
//! If pubread() or pubwrite() is not invoked fast enough, the timer may expire
//! even though sufficient number of bytes could've been read or written from
//! the file descriptor, otherwise.
//!
//! \par File descriptor terminator
//!
//! \code
//! INSERT_LIBX_NAMESPACE::eventfd ev;
//!
//! ev=eventfd::create();
//!
//! // ...
//!
//! socket_timeout->set_terminate_fd(ev);
//! \endcode
//!
//! set_terminate_fd() attaches another file descriptor. When the second
//! file descriptor becomes readable, the next pubread() or pubwrite() call to
//! the first file descriptor times out immediately, as if a timer was set, and
//! it timed out.
//!
//! The terminator descriptor may be an \ref eventfd "event file descriptor" or
//! a regular file descriptor that's a pipe or a socket, and attached to any
//! regular file, pipe, or a socket file descriptor.
//!
//! The same terminator file descriptor may be attached to multiple file
//! descriptor objects. When the terminator file descriptor becomes readable,
//! all file descriptor that it's attached to will throw timeouts.
//!
//! cancel_terminate_fd() detaches a terminator file descriptor.

class fdtimeoutObj : public fdbaseObj::adapterObj {

	//! The last read operation timed out

	bool timedout_read;

	//! The last write operation timed out

	bool timedout_write;

	//! Whether the read timer is set

	bool read_timer_set;

	//! The read timer
	timespec read_timer;

	//! Whether the write timer is set

	bool write_timer_set;

	//! The write timer

	timespec write_timer;

	//! Recurring read timer -- number of seconds

	timespec periodic_read_timeout;

	//! Recurring read timer -- how many bytes should be read before the timer expires

	size_t periodic_read_count;

	//! Recurring read timer -- bytes left before the timer gets reset

	size_t periodic_read_counter;

	//! Recurring write timer -- number of seconds

	timespec periodic_write_timeout;

	//! Recurring write timer -- how many bytes should be read before the timer expires

	size_t periodic_write_count;

	//! Recurring read timer -- bytes left before the timer gets reset

	size_t periodic_write_counter;

	//! Terminate file descriptor

	fdbaseptr terminatefdref;

public:
	//! Constructor

	fdtimeoutObj(//! The real file descriptor
		       const fdbase &ptr);

	//! Destructor
	~fdtimeoutObj();


	//! Set the terminating file descriptor

	//! When this file descriptor becomes readable, read(), write() and
	//! connect() will fail.
	//!
	//! \note
	//! This file descriptor must be nonblocking for this to work.

	void set_terminate_fd(const fdbase &fdRef)
	{
		terminatefdref=fdRef;
	}

	//! Remove the terminating file descriptor

	void cancel_terminate_fd()
	{
		terminatefdref=fdbaseptr();
	}

	//! Get the terminator filedescriptor

	fdbaseptr get_terminate_fd()
	{
		return terminatefdref;
	}

	//! Reset the periodic read timer and make it run again

	void reset_periodic_read_timer();

	//! Reset the periodic write timer and make it run again

	void reset_periodic_write_timer();

	//! Set a read timer.

	//! Use a read timer to force an error if the requisite data does
	//! not get read before a timer expires. For this to work this file
	//! descriptor must be nonblocking.
	//!
	void set_read_timeout(const timespec &timerref);

	//! Set a periodic read timer.

	//! This alternative function creates a timer file descriptor and
	//! uses it to force an error if the requisite
	//! number of bytes is not read within a predetermined time. If the
	//! requisite number of bytes is read, the timer gets reset again.
	//!
	//! \note
	//! This file descriptor must be nonblocking for this to work.
	void set_read_timeout(//! Number of bytes to read
			      size_t bytecnt,

			      //! How long to wait
			      const timespec &);

	//! Set a write timer.

	//! Use a write timer to force an error if the requisite data does
	//! not get written before a timer expires. For this to work this file
	//! descriptor must be nonblocking.

	void set_write_timeout(const timespec &timerref);

	//! Set a periodic write timer.

	//! This overloaded function creates a timer file descriptor and
	//! uses it to force an error if the requisite
	//! number of bytes is not written within the predetermined time. If the
	//! requisite number of bytes gets written, the timer is reset again.
	//!
	//! \note
	//! This file descriptor must be nonblocking for this to work.
	void set_write_timeout(//! Number of bytes to read
			      size_t bytecnt,

			      //! How long to wait
			      const timespec &spec);

	//! Cancel the read timer

	void cancel_read_timer();

	//! Cancel the write timer

	void cancel_write_timer();

	//! Implement the connect timeout

	void pubconnect(//! The server's address.
			const struct ::sockaddr *serv_addr,

			//! serv_addr's length, in bytes
			socklen_t addrlen) override;

	using fdbaseObj::adapterObj::pubaccept;

	//! Implement the accept timeout
	fdptr pubaccept(//! Connected peer's address
			sockaddrptr &peername) override;

	//! Implement the read timeout.

	size_t pubread(//! Buffer

		       char *buffer,

		       //! Element count

		       size_t cnt)
		LIBCXX_HIDDEN;

	//! Implement the write timeout

	size_t pubwrite(//! Buffer

			const char *buffer,

			//! Element count

			size_t cnt) LIBCXX_HIDDEN;

private:
	//! Wait for the file descriptor to be available for reading or writing

	//! \return \c true if the given timer has expired.
	//!

	bool wait_timer(//! Whether timer is initialized
			bool timer_set,

			//! What to wait for
			const timespec &timer,

			//! Event to wait for
			int events);

};

#if 0
{
#endif
}
#endif
