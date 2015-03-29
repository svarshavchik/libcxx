/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_fdtlsclientimpl_H
#define x_tls_fdtlsclientimpl_H

#include <x/namespace.h>
#include <x/gnutls/session.H>
#include <x/http/fdclientimpl.H>

namespace LIBCXX_NAMESPACE {

	class hms;

	namespace gnutls {

		//! SSL/TLS HTTP clients and servers

		//! This namespace implements HTTP clients and servers based
		//! on a file descriptor with a SSL/TLS session.

		namespace http {

#include <x/http/clientopts_t.H>

			class fdtlsclientimpl;
		};
	};
#if 0
};
#endif

//! An HTTP client on an SSL-encrypted file descriptor

//! A subclass of \c fdclientimpl that implements a TLS session.
//! The user is responsible for constructing a TLS session, and
//! invoking install(), which takes the following arguments:
//!
//! - The constructed TLS session.
//!
//! - Which file descriptor the TLS session gets connected to.
//!
//! - A terminator file descriptor. If the file descriptor becomes readable
//! the HTTP request gets aborted with an exception.
//!
//! - Additional options, a bitmask that contains the following values:
//!
//! - \c none - the default value.
//!
//! - \c isproxy - the file descriptor is a connection to an HTTP proxy.
//!
//! - \c noverifycert - do not check the server's certificate, at all.
//!
//! - \c noverifypeer - check the certificate, except for its hostname.
//! The hostname does not have to match the URI's host.
//!
//! The first request, after install(),
//! sent to the HTTP server results in TLS negotiations
//! taking place. A TLS shutdown sequence will happen if this class determines
//! that the connection is about to be shut down.
//!
//! Several properties govern additional timeouts for an SSL-encrypted
//! HTTP session.
//!
//! - \c INSERT_LIBX_NAMESPACE::https::client::handshake_timeout - the timeout
//! for negotiating a TLS session.
//!
//! - \c INSERT_LIBX_NAMESPACE::https::client::bye_timeout - the timeout
//! for terminating a TLS session.

class gnutls::http::fdtlsclientimpl : public LIBCXX_NAMESPACE::http::fdclientimpl {
	//! The \c INSERT_LIBX_NAMESPACE::https::client::handshake_timeout property
	static property::value<hms> handshake_timeout LIBCXX_INTERNAL;

	//! The \c INSERT_LIBX_NAMESPACE::https::client::bye_timeout property
	static property::value<hms> bye_timeout LIBCXX_INTERNAL;

	//! The current session

	sessionptr sess;

	//! The next request should result in a handshake taking place
	bool handshake_required;

private:
	//! Client options

	clientopts_t opts;

public:

	//! Default constructor - no associated connection
	fdtlsclientimpl();

	//! Destructor
	~fdtlsclientimpl() noexcept;

	//! Recycle the HTTP client

	//! Override install to save the proxy status of this connection

	void install(//! The TLS session
		     const session &sessArg,

		     //! The file descriptor
		     const fd &filedescArg,

		     //! The terminator file descriptor
		     const fdptr &terminatorArg,

		     //! Client options
		     clientopts_t optsArg);

	//! Override superclass

	//! It is an error to call this overloaded version through the
	//! superclass.

	void install(const fd &filedescArg,
		     const fdptr &terminatorArg);

	//! Override superclass

	//! It is an error to call this overloaded version through the
	//! superclass.

	void install(const fd &filedescArg,
		     const fdptr &terminatorArg,
		     clientopts_t options);

	//! Override clear() in the superclass.

	//! Also clear the session pointer, too.
	void clear();

	//! Use the current TLS session to read/write HTTP

	//! \internal
	//! (must be public for "make check")

	fdbase filedesc();

private:
	//! Perform handshake

	//! \internal
	void init(sendmsg &msg) LIBCXX_INTERNAL;

	//! Perform TLS connection termination

	//! \internal
	void ran() LIBCXX_HIDDEN;

	//! Check if a TLS handshake is required, before sending a request

	void internal_send(//! Request to be sent
			   sendmsg &msg) LIBCXX_HIDDEN;

	//! Flag - whether to passthrough the proxy error message

	bool proxyfailed;

	//! The error message from the proxy.
	LIBCXX_NAMESPACE::http::responseimpl proxyerror;

protected:

	//! Passthrough the proxy's error message as the response

	//! \internal
	//!
	void internal_get_message(LIBCXX_NAMESPACE::http::responseimpl &msg)
		LIBCXX_HIDDEN;

private:

	//! Attach session to the given file descriptor

	//! \internal
	void filedesc_installed(const fdbase &transport);
};

#if 0
{
#endif
}
#endif
