/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_fdserver_H
#define x_http_fdserver_H

#include <x/http/fdserverobj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

//! Implement a file descriptor-based HTTP server together with \ref fdlistener "INSERT_LIBX_NAMESPACE::fdlistener".

//! Derive a class from \ref fdserverimpl "INSERT_LIBX_NAMESPACE::http::fdserverimpl"
//! and \ref obj "INSERT_LIBX_NAMESPACE::obj", virtually, making it a
//! reference-counted object.
//!
//! Define a reference-counted class, a factory class, with a create()
//! method that instantiates the \c INSERT_LIBX_NAMESPACE::http::fdserverimpl
//! subclass.
//!
//! Start an \ref fdlistener "INSERT_LIBX_NAMESPACE::fdlistener" and pass an
//! instance of this \c INSERT_LIBX_NAMESPACE::http::fdserver as its first
//! argument, and the factory class as the second parameter.
//!
//! For each accepted connection, \c INSERT_LIBX_NAMESPACE::fdlistener run()s
//! this \c INSERT_LIBX_NAMESPACE::http::fdserver with forwarded arguments.
//! This class's run() method takes the first parameter, the factory class
//! instance, and invokes its create() method to construct a new instance
//! of the \c INSERT_LIBX_NAMESPACE::fdserverimpl subclass, install the file
//! descriptors, then invoke the constructed instance's run() method with any
//! remaining arguments from the original start(), so if the
//! \c INSERT_LIBX_NAMESPACE::fdserverimpl subclass implements its own
//! wrapper for run(), it can consume them, before invoking the run() defined by
//! \c INSERT_LIBX_NAMESPACE::fdserverimpl, which takes no arguments, which
//! implements HTTP.

typedef ref<fdserverObj> fdserver;

//! A nullable reference to a file descriptor-based HTTP server.

//! \see fdserver

typedef ptr<fdserverObj> fdserverptr;

#if 0
{
	{
#endif
	}
}
#endif
