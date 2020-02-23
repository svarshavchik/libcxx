/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_fdserverimpl_H
#define x_http_fdserverimpl_H

#include <x/http/fdimplbase.H>
#include <x/http/serverimpl.H>
#include <x/property_value.H>
#include <iterator>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	class hms;

	namespace http {
#if 0
	};
};
#endif

//! An HTTP server on a file descriptor

//! Create a reference-counted factory object with a create() method that
//! constructs a subclass of this class and
//! \ref obj "INSERT_LIBX_NAMESPACE::obj".
//!
//! Use \ref fdlistener "INSERT_LIBX_NAMESPACE::fdlistener" to listen for
//! incoming connections, with two argument to its start():
//! an instance of \ref fdserver "INSERT_LIBX_NAMESPACE::http::fdserver" and
//! an instance of the factory class.
//!
//! \ref fdlistener "INSERT_LIBX_NAMESPACE::fdlistener" calls its first
//! argument, the \ref fdserver "INSERT_LIBX_NAMESPACE::http::fdserver"
//! instance, for each accepted incoming connection. It calls the factory's
//! create() method to construct a new instance of the server class, the
//! factory returns an instance of the subclass of this class, which runs
//! the HTTP server.
//!
//! An instance of this class supports only one server thread. Multiple
//! server threads cannot run the same instance of this class. A factory
//! object, in this manner, is required to construct a new instance for each
//! connection.
//!
//! run() returns when the HTTP client disconnects. HTTP 1.1 clients may
//! send multiple requests, HTTP 1.0 always send one request only.
//!
//! The subclass can define its own %run() method that takes additional
//! arguments, which wraps this run() method. The additional arguments get
//! forwarded through fdlistener.
//!
//! The subclass must define received() (implemented in the superclass)
//! to implement each HTTP request.
//!
//! Most of the methods get inherited from the underlying transport-independent
//! HTTP server class. It is privately inherited, and only those methods that
//! a subclass can use are exposed to the subclass:
//!
//! received() may invoke begin() and end() to retrieve the request message's
//! body. Alternatively, if the request has a body but the subclass doesn't
//! want it, discard() throws it away. This happens automatically, if received()
//! returns without consuming the body. However, discard() explicitly reads it
//! from the client. received() gets invoked before the request message's
//! content is read. begin() and end() return \c iterator instances that
//! actually read from the underlying file descriptor (and may encounter
//! network delays). discard() consumes the input, so the sending client
//! doesn't timeout sending its HTTP request content (which typically has
//! a shorter timeout), and can start waiting for a response (which typically
//! has a longer timeout).
//!
//! getform() is a convenient way to retrieve a
//! \ref form "INSERT_LIBX_HTTP_NAMESPACE::http::form" from the current request.
//! For POSTed forms, getform() consumes the content of the request. For a GET
//! request, getform() consumes the "query string" in the URI (which is also
//! consumed and merged with the POSTed content, if it exists in a POST
//! mesasge).
//!
//! received() must invoke send() exactly once in order to send the response
//! to the client, and terminate. As an alternative, received() may invoke
//! one of the "throw" functions defined in responseimpl to throw an
//! %exception that ends up sending a standatd HTTP response to the client.
//!
//! Alternative, construct an exception object yourself, and invoke
//! report_exception(), instead of send(), to respond with an HTTP exception.
//!
//! Other exposed methods are for the TLS server subclass, and should not
//! be used.
//!
//! Several properties govern the limits and timeouts for a file
//! descriptor-based HTTP server:
//!
//! - \c INSERT_LIBX_NAMESPACE::http::server::pipeline_timeout - the timeout
//! to wait
//! for the client to begin sending a message. The connection gets closed as
//! inactivity, and no error gets reported, if the timeout expires.
//!
//! - \c INSERT_LIBX_NAMESPACE::http::server::headers::timeout - the timeout for
//! receiving an HTTP header.
//!
//! - \c INSERT_LIBX_NAMESPACE::http::server::headers::maxsize - maximum HTTP
//! message
//! header size.
//!
//! - \c INSERT_LIBX_NAMESPACE::http::server::headers::limit - maximum number
//! of headers in an HTTP message.
//!
//! - When receiving the body of the HTTP request message, or sending the
//! HTTP response message if
//! \c INSERT_LIBX_NAMESPACE::http::server::body_timeout_cnt bytes are not
//! received/sent in
//! \c INSERT_LIBX_NAMESPACE::http::server::body_timeout seconds, an internal
//! %exception gets thrown by begin(), end(), or send().

class fdserverimpl : private fdimplbase,
		     private serverimpl<fdimplbase::input_iter_t,
					fdimplbase::output_iter_t> {

	//! Server superclass type
	typedef serverimpl<input_iter_t, output_iter_t> superclass_t;

	static property::value<hms>
	//! The \c INSERT_LIBX_NAMESPACE::http::server::pipeline_timeout property
	pipeline_timeout LIBCXX_HIDDEN,

	//! The \c INSERT_LIBX_NAMESPACE::http::server::headers::timeout property
		headers_timeout LIBCXX_HIDDEN,

	//! The \c INSERT_LIBX_NAMESPACE::http::server::body_timeout property
		body_timeout LIBCXX_HIDDEN;

	static property::value<size_t>
	//! The \c INSERT_LIBX_NAMESPACE::http::server::headers::maxsize property
	headers_maxsize,
	//! The \c INSERT_LIBX_NAMESPACE::http::server::headers::limit property
		headers_limit,
	//! The \c INSERT_LIBX_NAMESPACE::http::server::body_timeout_cnt property
		body_timeout_cnt LIBCXX_HIDDEN;

protected:
	//! Convenience typedef

	typedef superclass_t::iterator iterator;

	using superclass_t::begin;
	using superclass_t::end;
	using superclass_t::send;
	using superclass_t::discardbody;
	using superclass_t::getform;
	using superclass_t::report_exception;
	using superclass_t::received_unknown_request;

	using fdimplbase::getconnection;
	using fdimplbase::set_readwrite_timeout;
	using fdimplbase::cancel_readwrite_timeout;

	class LIBCXX_HIDDEN run_helper;

	//! Invoked by run_helper, before returning from run()

	//! \internal
	void clear() override;

	//! For use by regression tests

	//! \internal
	void debug_install_fd(const fd &dummy_fd)
	{
		fdimplbase::install(dummy_fd, fdptr());
	}

public:
	friend class run_helper;

	//! Constructor
	fdserverimpl();

	//! Destructor
	~fdserverimpl();

	//! Run a server.

	//! Invokes the superclass's %run().
	//! Any response_exception thrown in the superclass's %run() gets
	//! quietly trapped and ignored. It's probably a timeout exception.

	void run(//! File descriptor for the connection from the client
		 const fd &connection,

		 //! Terminator file descriptor for the connection, if any.

		 const fd &terminator);

private:

	//! Overridden by fdtlsclientimpl to bye() the session

	virtual void ran();

	//! Override the superclass's method to check for a pipeline timeout

	//! \internal
	//!

	bool eof() override;

private:
	//! Begin header retrieval with a timeout
	void header_begin() override;

	//! Check for a previous timeout before beginning to retrieve the message body

	//! \internal
	//!

	void body_begin() override;

	//! Check for a timeout after retrieving the message body

	//! \internal
	//!

	void body_end() override;

	//! Subclass hook to set any timeouts.
	void begin_write_message() override;

	//! Subclass hook to clear any timeouts
	void end_write_message() override;

};


#if 0
{
	{
#endif
	}
}
#endif
