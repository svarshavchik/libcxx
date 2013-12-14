/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_sessioncachefwd_H
#define x_tls_sessioncachefwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>
#include <x/exception.H>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {

#if 0
	};
};
#endif

class sessioncacheBase;
class sessioncacheObj;

//! A TLS/SSL session cache

//! \code
//!
//! INSERT_LIBX_NAMESPACE::gnutls::sessioncache cache=INSERT_LIBX_NAMESPACE::gnutls::sessioncache::create();
//!
//! sess->session_cache(cache);
//! \endcode
//!
//! Implements a TLS/SSL session cache. create() constructs a default
//! implementation that maintains a TLS cache in memory. Custom subclasses of
//! \ref sessioncacheObj "INSERT_LIBX_NAMESPACE::gnutls::sessioncacheObj" can
//! implement custom session cache stores.
//!
//! Subclass sessioncacheObj and implement the store(), remove() and retr()
//! methods.

typedef ref<sessioncacheObj, sessioncacheBase> sessioncache;

//! A nullable pointer reference to a sessioncache object

//! \see sessioncache
typedef ptr<sessioncacheObj, sessioncacheBase> sessioncacheptr;

#if 0
{
	{
#endif
	}
}
#endif
