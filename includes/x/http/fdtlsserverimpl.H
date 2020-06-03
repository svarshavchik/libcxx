/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_fdtlsserverimpl_H
#define x_tls_fdtlsserverimpl_H

#include <x/namespace.h>
#include <x/hmsfwd.H>
#include <x/gnutls/session.H>
#include <x/http/fdserverimpl.H>

namespace LIBCXX_NAMESPACE::gnutls::http {
	class fdtlsserverimpl;
}

namespace LIBCXX_NAMESPACE {
#if 0
}
#endif
//! An HTTP-over-SSL server on a file descriptor

//! This is a version of \ref fdserverimpl "fdserverimpl" that's used with
//! \ref fdtlsserver "fdtlsserver" to implement HTTP over TLS/SSL.
//!
//! Derive from this subclass in order to implement an HTTP server that uses
//! TLS/SSL.

class gnutls::http::fdtlsserverimpl : public LIBCXX_NAMESPACE::http::fdserverimpl {
	//! The \c INSERT_LIBX_NAMESPACE::https::server::handshake_timeout property
	static property::value<hms> handshake_timeout LIBCXX_INTERNAL;

	//! The \c INSERT_LIBX_NAMESPACE::https::server::bye_timeout property
	static property::value<hms> bye_timeout LIBCXX_INTERNAL;

	//! The associated TLS session
	sessionptr sess;

public:
	//! Default constructor
	fdtlsserverimpl();

	//! Destructor
	~fdtlsserverimpl();

	//! Install the factory for TLS sessions.

	//! Invoke the superclass's %run()

	//! Perform a TLS handshake, process HTTP requests, then attempt to
	//! terminate the TLS session

	void run(//! File descriptor for the connection from the client
		 const fd &connection,

		 //! Terminator file descriptor for the connection, if any.

		 const fd &terminator,

		 //! The TLS session

		 const session &sessionArg);

	//! Use the current TLS session to read/write HTTP

	fdbase filedesc() override;

	class LIBCXX_HIDDEN cleanup_helper;

	friend class cleanup_helper;

private:
	//! Perform TLS connection termination

	//! \internal
	void ran() override;

	//! Internal cleanup function used by run()

	//! \internal
	void clear() override;

	//! Perform TLS handshake

	//! \internal

	void filedesc_installed(const fdbase &transport) override;
};

#if 0
{
#endif
}
#endif
