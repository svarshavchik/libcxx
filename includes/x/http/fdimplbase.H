/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_fdimplbase_H
#define x_http_fdimplbase_H

#include <x/fdbasefwd.H>
#include <x/fdfwd.H>
#include <x/fditer.H>
#include <x/fdreadlimit.H>
#include <x/fdtimeout.H>
#include <x/namespace.h>

#include <iterator>

namespace LIBCXX_NAMESPACE {

	class timerfdObj;

	namespace http {
#if 0
	};
};
#endif

#include <x/http/clientopts_t.H>

//! Common base class for file descriptor-based HTTP clients and servers

//! Stores the underlying file descriptor structures. They are not
//! initialized by the constructor. Rather, the subclass invokes
//! install() when it figures out where the file descriptor comes from.

class fdimplbase {

public:
	//! Throw a timeout

	static void throw_request_timeout()
		__attribute__((noreturn));

	//! Original file descriptor

	fdptr orig_filedesc;

	//! The file descriptor interface with a read limit implementation

	fdreadlimitptr filedesc_readlimit;

	//! The file descriptor interface with a timeout implementation

	//! Non-encrypted HTTP clients and servers instantiate input and
	//! output iterators that read and write HTTP messages to this
	//! file descriptor, which is tied to fdreadlimitptr and
	//! orig_filedesc.
	//!
	//! Encrypted HTTP clients and servers instantiate input and
	//! output iterators that are tied to the HTTP session object,
	//! which is tied to this file descriptor transport.

	fdtimeoutptr filedesc_timeout;

	//! Clear all these file descriptor

	virtual void clear();

	//! The file descriptor object to read from

	//! This is used to construct the input iterators for the http
	//! stream. fdimplbase returns filedesc_timeout by default.
	//! This gets overriden in the TLS classes to return, instead
	//! the TLS session.

	virtual fdbase filedesc();

	//! Define an input iterator for reading from the file descriptor

	//! \internal
	//!

	class input_iter_t : public fdinputiter {

	public:
		//! Constructor
		input_iter_t(const fdbase &fdArg);

		//! Constructor
		input_iter_t();

		//! Destructor
		~input_iter_t();

#ifndef DOXYGEN
		// TODO - doxygen fix

		using fdinputiter::operator==;
		using fdinputiter::operator!=;
#endif

		//! Iterator operator
		input_iter_t &operator++()
		{
			fdinputiter::operator++();
			return *this;
		}

		//! Iterator operator
		const char *operator++(int)
		{
			return fdinputiter::operator++(0);
		}

		//! Override pubread()

		//! When reading from the transport, catch ETIMEDOUTs thrown
		//! by fdtimeoutObj, and invoke throw_request_timeout().

		size_t pubread(const fdbase &fdref, char *ptr, size_t cnt)
			const override;

	};

	//! Define an output iterator for reading from the file descriptor

	//! \internal
	//!

	class output_iter_t : public fdoutputiter {
	public:

		//! Constructor
		output_iter_t(const fdbase &fdArg);

		//! Constructor
		output_iter_t();

		//! Destructor
		~output_iter_t();

		//! Iterator operator
		output_iter_t &operator++()
		{
			fdoutputiter::operator++();
			return *this;
		}

		//! Iterator operator
		output_iter_t &operator++(int)
		{
			fdoutputiter::operator++(0);
			return *this;
		}

		//! Iterator operator
		output_iter_t &operator*()
		{
			fdoutputiter::operator*();
			return *this;
		}

		//! Iterator operator
		output_iter_t &operator=(char c)
		{
			fdoutputiter::operator=(c);
			return *this;
		}

		//! Iterator operator

		void flush() override;
	};

	//! Constructor
	fdimplbase();

	//! Install a new file descriptor
	virtual void install(const fd &filedescArg,
			     const fdptr &terminatefdArg);

	//! Destructor
	~fdimplbase();

	//! Set the terminating file descriptor

	void set_terminate_fd(const fdbase &fd)
	{
		filedesc_timeout->set_terminate_fd(fd);
	}

	//! Remove the terminating file descriptor

	void cancel_terminate_fd()
	{
		filedesc_timeout->cancel_terminate_fd();
	}

	//! Set read and write timeout (used by fdtlsserverimpl)

	void set_readwrite_timeout(time_t seconds);

	//! Cancel read and write timeouts (used by fdtlsserverimpl)

	void cancel_readwrite_timeout();

	//! Return the underlying connection socket.

	const_fdptr getconnection() const
	{
		return orig_filedesc;
	}

	//! Borrow the connection from somebody else

	//! This is used by TLS proxy setup.

	void borrow(const fdimplbase &other);

private:

	//! Callback: a file descriptor has been installed

	//! This gets invoked by install(). It's a callback that's used by
	//! fdtlsclientimpl and fdtlsserverimpl. They need to do additional
	//! stuff after the transport file descriptor gets installed.

	virtual void filedesc_installed(const fdbase &transport);
};

#if 0
{
	{
#endif
	}
}
#endif
