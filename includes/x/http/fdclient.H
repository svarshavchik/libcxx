/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_fdclient_H
#define x_http_fdclient_H

#include <x/http/fdclientobj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

class fdclientObj;

//! A file descriptor-based HTTP client

//! \code
//! http::fdclient cl(fdclient::create());
//!
//! cl->install(netaddr::create("www.example.com", "http")->connect()));
//!
//! http::requestimpl req;
//!
//! http::responseimpl resp;
//!
//! req.setURI("http://www.example.com");
//!
//! if (cl->send(req, resp))
//! {
//!    if (req.responseHasMessageBody(resp))
//!    {
//!        http::fdclientimpl::iterator b(cl->begin()), e(cl->end());
//!
//!        std::copy(b, e, std::ostreambuf_iterator<char>(std::cout));
//!    }
//! }
//! \endcode

typedef ref<fdclientObj> fdclient;

//! A nullable reference to a file descriptor-based HTTP client

//! \see fdclient

typedef ptr<fdclientObj> fdclientptr;

#if 0
{
	{
#endif
	}
}
#endif
