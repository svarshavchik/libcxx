/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_cgiimpl_H
#define x_http_cgiimpl_H

#include <x/http/requestimpl.H>
#include <x/http/clientimpl.H>
#include <x/http/responseimpl.H>
#include <x/http/form.H>
#include <x/fditer.H>
#include <x/basicstreamobj.H>
#include <x/sockaddrfwd.H>
#include <x/namespace.h>

#include <map>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

//! Common Gateway Interface environment when invoked from a web server

//! Instantiate this object, then retrieve method, script_name, path_info, and
//! and scheme to determine this application's invocation parameters.
//! cgiimpl is derived from headerbase, which contains the HTTP headers
//! from the HTTP request.
//!
//! If the application was invoked to process a form, the parameters map
//! contains the form's parameters.
//!
//! hasData() indicates whether a this application was invoked to process
//! an arbitrary object. It returns \c true if the request method is
//! a POST or a PUT. Instantiate a
//! \ref mime::structured_content_header "INSERT_LIBX_NAMESPACE::mime::structured_content_header"
//! for the "Content-Type" header, to retrieve its MIME
//! type, then use data_begin()
//! and data_end() to retrieve the iterators over the object body bytestream.
//!
//! A cgiimpl object may be instantiated only once. Second and subsequent
//! cgiimpl objects will not have access to the HTTP request's body. If the
//! application was invoked to process a web form submitted via a \c POST,
//! the form parameters will also not be available.

class cgiimpl {

public:
	//! The request's HTTP method, usually GET or POST
	method_t method;

	//! The script's host
	std::string host;

	//! The script's name
	std::string script_name;

	//! Any extra path in the URL, after the script's name
	std::string path_info;

	//! Either "http" or "https"
	std::string scheme;

	//! Reconstructed from REMOTE_ADDR and REMOTE_PORT
	sockaddrptr remote_addr;

	//! Reconstructed from SERVER_ADDR and SERVER_PORT
	sockaddrptr server_addr;

	//! Any headers
	headersbase headers;

	//! Parsed form parameters

	form::parameters parameters;

	//! getURI() will produce a relative URL.

	//! The URI returned by getURI will have just the last component of the
	//! combined script_name+path_info path.
	//!
	//! If this flag is not specified, getURI() produces an absolute URI.

	static constexpr int uri_relative=1;

	//! getURI() includes the path_info.

	//! Mutually-exclusive with uri_relative. Includes the extra
	//! path_info in the URI.

	static constexpr int uri_path=2;

	//! getURI() returns a URI that includes all parameters.

	//! Put all parameters into the query_string part of the returned URI.
	//! The resulting URI should only be used for a GET.
	//!
	static constexpr int uri_query=4;

	//! Reconstruct the CGI request's URI.

	//! This URI can be used to refer to this same application, in
	//! dynamically-generated content.

	uriimpl getURI(//! Options that specify which parts of the URI should
		       //! be constructed. Default: produce an absolute URI,
		       //! without any extra path_info, and without any
		       //! parameters
		       int options=0) const;

private:

public:
	//! The iterator over the HTTP message content

	typedef fdinputiter iterator;

	//! Default constructor

	//! Only the first instance of a \c cgiimpl will have all the CGI
	//! parameters.
	cgiimpl(//! Whether to process application/x-form-www-urlencoded
		//! POST, and initialize them to parameters.
		//!
		//! With proxymode set to \c false this is what will happen,
		//! method gets changed to GET, and the Content-Length: gets
		//! removed.
		//!
		//! With proxymode set to \c true, the POST is left alone,
		//! as is.
		bool proxymode=false);

	//! Destructor
	~cgiimpl();

	//! Check whether an object was submitted to this application

	//! \return \c true if data_begin() and data_end() may be invoked
	//! to obtain the object's iterator
	bool hasData();

	//! Obtain the beginning iterator for the object that was submitted to this application.

	//! Only valid if hasData() returns \c true. Otherwise, the returned
	//! iterator will be data_end().

	iterator begin();

	//! Obtain the beginning iterator for the object that was submitted to this application.

	iterator end();

	//! Format a CGI response.

	//! A CGI-formatted response is sent. Only the headers are sent,
	//! followd by a blank line. The application should then send the
	//! response body to \c std::cout.
	//!
	static void send_response_header(responseimpl &resp);
};

#ifndef DOXYGEN
#define LIBCXX_TEMPLATE_DECL extern

#include <x/http/cgiimpl_t.H>
#undef LIBCXX_TEMPLATE_DECL
#endif

#if 0
{
	{
#endif
	}
}
#endif
