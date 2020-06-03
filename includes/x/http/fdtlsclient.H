/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_fdtlsclient_H
#define x_http_fdtlsclient_H

#include <x/http/fdtlsclientobj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::gnutls::http {
#if 0
}
#endif

class fdtlsclientObj;

//! A file descriptor-based HTTPS client

//! \code
//! gnutls::credentials::certificate cred=
//!     gnutls::credentials::certificate::create();
//!
//! cred->set_x509_trust_default();
//!
//! gnutls::http::fdtlsclient cl(gnutls::http::fdtlsclient::create());
//!
//! cl->install_credentials(cred);
//!
//! cl->install(netaddr::create("www.example.com", "https"), none);
//! \endcode
//!
//! The second parameter to install is a bitmask of the following values:
//!
//! - \c none - the default value.
//!
//! - \c isproxy - the file descriptor is a connection to an HTTP proxy.
//!
//! - \c noverifycert - do not check the server's certificate, at all.
//!
//! - \c noverifypeer - check the certificate, except for its hostname.
//! The hostname does not have to match the URI's host.

typedef ref<fdtlsclientObj> fdtlsclient;

//! A nullable pointer reference to a file descriptor-based TLS client.

//! \see fdtlsclient

typedef ptr<fdtlsclientObj> fdtlsclientptr;

#if 0
{
#endif
}
#endif
