/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fdbaseobj_H
#define x_fdbaseobj_H

#include <x/obj.H>
#include <x/ptr.H>

#include <iostream>
#include <x/ref.H>
#include <x/basicstreamobj.H>
#include <x/sockaddr.H>
#include <x/property_valuefwd.H>
#include <x/sys/offt.h>
#include <sys/socket.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class fdstreambufObj;

//! File descriptor interface

//! This base class defines five methods, pubread(), pubwrite(), pubseek(),
//! connect(), and getFd(), which must be implemented in its subclass.
//!
//! Additional methods are also defined that operate on the underlying file
//! descriptor.

class fdbaseObj : virtual public obj {

public:
	//! The default buffer size property, for serializing

	static property::value<size_t> buffer_size LIBCXX_HIDDEN;

	class adapterObj;

	//! Default constructor
	fdbaseObj() noexcept {}

	//! Default destructor
	~fdbaseObj() noexcept {}

	//! Return the underlying file descriptor

	//! \return the currently open file descriptor, or -1.
	//!
	virtual int getFd() const noexcept=0;

	//! Connect the socket

	//! pubconnect() makes a client socket connection to a server, and
	//! is a wrapper for the connect(2) system call.

	virtual void pubconnect(//! The server's address.

				const struct ::sockaddr *serv_addr,

				//! serv_addr's length, in bytes

				socklen_t addrlen)=0;

	//! Connect the socket

	//! \overload
	//!
	void pubconnect(//! The server's address.

			const const_ptr<sockaddrObj> &addr)
	{
		pubconnect( (const struct ::sockaddr *)&*addr->begin(),
			    addr->size());
	}

	//! Read from the underlying file descriptor

	//! The possible outcomes are:
	//!
	//! - an error occurs: an %exception gets thrown
	//!
	//! - at least one byte was read: the number of bytes read gets
	//! returned.
	//!
	//! - an end-of-file condition: 0 is returned, errno is 0
	//!
	//! - no bytes were read from a non-blocking socket: 0 is returned,
	//! errno is non-0.

	virtual size_t pubread(//! Buffer

			       char *buffer,

			       //! Element count

			       size_t cnt)
=0;

	//! Check if there's unread data waiting in the filedescriptor

	//! When using a layer on top of the file descriptor, this call
	//! checks whether the layer has a read buffer with data that's unread,
	//! and the next pubread() will not block.
	//!
	//! \return the number of unread bytes. Invoking pubread() with a
	//! count that does not exceed this count is guaranteed not to block.
	//! A return value of zero means that there are no unread bytes, or
	//! its unknown.

	virtual size_t pubread_pending() const=0;

	//! Write to the underlying file descriptor

	//! \return number of bytes written, which may be 0 if the underlying
	//! file descriptor is non-blocking, and writing to the file descriptor
	//! would block. An error in the underlying transport is reported by
	//! throwing an %exception.

	virtual size_t pubwrite(//! Buffer

				const char *buffer,

				//! Element count

				size_t cnt)=0;

	//! Seek to a new position for the file descriptor.

	//! Errors are reported by throwing an %exception.
	//!
	virtual off64_t pubseek(//! Offset.

				off64_t offset,

				//! Either \c SEEK_SET, \c SEEK_CUR, or \c SEEK_END.

				int whence)=0;

	//! Return a stream buffer for this file descriptor

	//! \return a reference to a \c std::basic_streambuf subclass that
	//! reads or writes from this file descriptor.
	//!
	//! \note
	//! The stream buffer must remain in scope as long as this file
	//! descriptor remains open. If the stream buffer goes out of scope,
	//! unread and/or unflushed data will get lost.
	//!
	//! As an alternative to a full-fledged stream buffer, use
	//! \ref fdBase::inputiter "fd::inputiter" or
	//! \ref fdBase::outputiter "fd::inputiter" as lightweight
	//! input or output iterators that read or write from a buffered
	//! file descriptor interface.

	ref<fdstreambufObj> getStreamBuffer(size_t bufsiz=get_buffer_size());

	//! Retrieve the default buffer size file

	static size_t get_buffer_size();

	//! Throw an %exception when connect() fails

	static void badconnect(//! The server's address.

			       const struct ::sockaddr *serv_addr,

			       //! serv_addr's length, in bytes

			       socklen_t addrlen)
		LIBCXX_HIDDEN __attribute__((noreturn));

	//! Return a stream associated with this file descriptor

	//! This template function constructs a stream object associated with
	//! this file descriptor. The template parameter is a stream class,
	//! that specifies the stream class to return.
	//!
	//! \note
	//! The stream must remain in scope as long as this file
	//! descriptor remains open. If the stream goes out of scope,
	//! unread and/or unflushed data will get lost.
	//!
	//! \see getistream(), getostream(), getiostream(), getwistream(), getwostream(), getwiostream().

	template<typename streamRefType> streamRefType getStream()

	{
		return streamRefType::create(getStreamBuffer());
	}

	//! Return the input stream associated with this file descriptor

	//! This is a convenience function that uses getStream() to return
	//! an istream object for this file descriptor.
	//!
	//! Use
	//! \ref fdBase::inputiter "fd::inputiter"  as a lightweight
	//! input iterator over a file descriptor, if a full-fledged
	//! input stream is not required.

	istream getistream();

	//! Return the output stream associated with this file descriptor

	//! This is a convenience function that uses getStream() to return
	//! an ostream object for this file descriptor.
	//!
	//! Use
	//! \ref fdBase::outputiter "fd::outputiter" as a lightweight
	//! output iterator over a file descriptor, if a full-fledged
	//! input/output stream is not required.

	ostream getostream();

	//! Return the stream associated with this file descriptor

	//! This is a convenience function that uses getStream() to return
	//! an iostream object for this file descriptor.
	//!
	//! Use
	//! \ref fdBase::inputiter "fd::inputiter" and
	//! \ref fdBase::outputiter "fd::outputiter" as lightweight
	//! (and independent) input and output iterators
	//! over a file descriptor, when a full-fledged
	//! input/output stream is not required.
	//!
	//! \note
	//! Separately-instantiated input and output iterators are completely
	//! independent from each other, and have individual buffers.
	//! Reading from an input iterator will not flush the output iterator,
	//! and writing to an output iterator will not unbuffer the input
	//! iterator.

	iostream getiostream();

};

//! A reference to a file descriptor-like based transport.

typedef ref<fdbaseObj> fdbase;

//! A reference to an immutable file descriptor-like based transport.

typedef const_ref<fdbaseObj> const_fdbase;

//! A nullable reference to a file descriptor-like based transport.

typedef ptr<fdbaseObj> fdbaseptr;

//! A nullable reference to an immutable file descriptor-like based transport.

typedef const_ptr<fdbaseObj> const_fdbaseptr;

//! An fdbase that chains to another fdbase

//! This is used to stack timeout logic for reading/writing from another
//! fdbase. Provide a default implementation for all virtual methods.

class fdbaseObj::adapterObj : public fdbaseObj {

protected:
	//! The real fdbase

	fdbase fdptr;

public:
	//! Constructor

	adapterObj(//! The real fdbase
		   const fdbase &ptrArg);

	//! Destructor

	~adapterObj() noexcept;

	//! Pass through method

	//! The default adapter invokes ptr->connect().
	//!

	virtual void pubconnect(//! The server's address.

				const struct ::sockaddr *serv_addr,

				//! serv_addr's length, in bytes

				socklen_t addrlen);

	//! Pass through method

	//! The default adapter invokes ptr->getFd().
	//!

	int getFd() const noexcept;

	//! Pass through method

	//! The default adapter invokes ptr->pubread().
	//!

	size_t pubread(//! Buffer

		       char *buffer,

		       //! Element count

		       size_t cnt)
;

	//! Pass through method

	//! The default adapter invokes ptr->pubread_pending().
	//!

	size_t pubread_pending() const;

	//! Pass through method

	//! The default adapter invokes ptr->pubwrite().
	//!
	size_t pubwrite(//! Buffer

			const char *buffer,

			//! Element count

			size_t cnt);

	//! Pass through method

	//! The default adapter invokes ptr->pubseek().
	//!
	off64_t pubseek(//! Offset.

			off64_t offset,

			//! Either \c SEEK_SET, \c SEEK_CUR, or \c SEEK_END.

			int whence);
};

#if 0
{
#endif
}
#endif
