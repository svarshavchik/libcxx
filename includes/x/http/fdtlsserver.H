/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_fdtlsserver_H
#define x_http_fdtlsserver_H

#include <x/http/fdtlsserverobj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
	namespace gnutls {
		namespace http {
#if 0
		};
	};
};
#endif

//! Implement a file descriptor-based HTTPS server together with \ref fdlistener "INSERT_LIBX_NAMESPACE::fdlistener".

//! This is similar to \ref http::fdserver "INSERT_LIBX_NAMESPACE::http::fdserver", with
//! the differences as follows:
//!
//! - Pass an instance of this \c INSERT_LIBX_NAMESPACE::http::fdtlsserver as
//! the first argument to \c INSERT_LIBX_NAMESPACE::fdlistener's start().
//!
//! - The second and the third arguments are two factory objects.
//! The first factory's create() method gets called with two parameters,
//! \c GNUTLS_SERVER, and the connected socket; its create() method is
//! expected to return a suitable
//! \ref gnutls::session "INSERT_LIBX_NAMESPACE::gnutls::session".
//! The second factory's create() takes no parameters. It is expected to
//! construct a subclass of
//! \ref fdtlsserverimpl "INSERT_LIBX_NAMESPACE::http::gnutls::fdtlsserverimpl".
//!
//! - Any remaining arguments get forwarded to the run() method of the object
//! constructed by the second factory.
//! The default run() defined by its
//! "INSERT_LIBX_NAMESPACE::http::gnutls::fdtlsserverimpl"
//! superclass takes no argument. The subclass constructed by the factory
//! object can implement its own wrapper for run() that consumes additional
//! arguments, before invoking the superclass's run().
//!
//! This server object implements the run() method that gets invoked by
//! "INSERT_LIBX_NAMESPACE::fdlistener". It calls both factories, installs
//! the TLS session and the file descriptors into
//! the \c INSERT_LIBX_NAMESPACE::http::gnutls::fdtlsserverimpl superclass.
//!
//! \code
//! class myCacheObj : LIBCXX_NAMESPACE::gnutls::sessioncacheObj {
//! ...
//! };
//!
//! auto cache=LIBCXX_NAMESPACE::ref<myCacheObj>::create();
//!
//! auto server=LIBCXX_NAMESPACE::gnutls::http::fdtlsserver::create(cache);
//! \endcode
//!
//! An optional argument to the constructor installs a custom
//! \ref sessioncache "TLS session cache implementation". The default
//! constructor installs a default, in memory, TLS session cache.

typedef ref<fdtlsserverObj> fdtlsserver;

//! A nullable pointer reference to an HTTPS server

//! \see fdtlsserver

typedef ptr<fdtlsserverObj> fdtlsserverptr;

#if 0
{
	{
		{
#endif
		}
	}
}
#endif
