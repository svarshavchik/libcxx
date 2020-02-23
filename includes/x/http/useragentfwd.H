/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_useragentfwd_H
#define x_http_useragentfwd_H

#include <x/ref.H>
#include <x/obj.H>
#include <x/property_value.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

class useragentObj;
class useragentBase;

//! A high level HTTP useragent

//! This object implements a generic HTTP client. Persistent connections to
//! HTTP servers that implement HTTP 1.1 are cached, where possible.
//! This is a reference to a reference-counted object that, once created,
//! can be passed around freely.
//!
//! \code
//! http::useragent::https_enable();
//! \endcode
//!
//! https_enable() is not required when linking with shared libraries, by
//! default. Invoking https_enable() is required only when linking statically,
//! in order to load the https-enabled module.
//!
//! \code
//! http::useragent ua(http::useragent::create());
//!
//! http::responseimpl resp;
//!
//! req.setURI("http://www.example.com");
//!
//! http::useragent::base::response resp=ua->request(req);
//!
//! if (resp->has_content())
//! {
//!     http::useragent::base::response::iterator
//!         b=resp->begin(),
//!         e=resp->end();
//!     dosomething(b, e);
//! }
//! if (resp->getStatusCode(0 != 200)
//!     throw EXCEPTION("Request failed");
//! 
//! \endcode
//!
//! This example sends an \c HTTP \c GET request. request() returns a
//! \ref responseimpl responseimpl class, as well as a beginning and an ending
//! iterator that define the content, if any, of the received response.
//! has_content() specifies whether the response to the HTTP request returns
//! content. Generally, most responses, except HEADs, return content.
//!
//! \code
//! std::string text("Hello world");
//!
//! http::requestimpl req;
//!
//! req.setMethod(req.POST);
//! req.setURI("http://www.example.com/submit.cgi");
//!
//! http::useragent::base::response resp=ua->request(req, text.begin(), text.end());
//!
//! std::string document(resp->begin(), resp->end());
//!
//! \endcode
//!
//! A request that includes a document, such as a \c POST, is sent by passing
//! a beginning iterator and an ending iterator to request(), which also
//! returns a response object, with a begin() and an end() that return iterators
//! for the returned content.
//!
//! begin() and end() returns the same ending iterator value for responses that
//! do not include content. It is safe to invoke them after obtaining any
//! response object, however they can only be invoked at most once, for each
//! particular response object.
//!
//! \note
//! The iterators given to request(), that define a document to be sent with
//! an HTTP request, must be, at least, forward iterators. They cannot be only
//! input iterators.
//!
//! \note
//! Persistent HTTP connections get reused for multiple HTTP requests to the
//! same server only if response objects, and response iterators
//! are properly discarded, and go out of scope. Until they do, they hold the
//! connection with the server, which cannot be reused for other HTTP requests
//! until all objects associated with it go out of scope.

typedef ref<useragentObj, useragentBase> useragent;

//! A nullable pointer reference to a high level HTTP user agent

//! \see useragent

typedef ptr<useragentObj, useragentBase> useragentptr;

//! A nullable reference to a const high level HTTP user agent

//! \see useragent

typedef const_ref<useragentObj, useragentBase> const_useragent;

//! A nullable pointer reference to a const high level HTTP user agent

//! \see useragent

typedef const_ptr<useragentObj, useragentBase> const_useragentptr;
#if 0
{
	{
#endif
	}
}
#endif
