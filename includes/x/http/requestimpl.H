/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_requestimpl_H
#define x_http_requestimpl_H

#include <x/http/messageimplfwd.H>
#include <x/uriimpl.H>
#include <x/namespace.h>

#include <map>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

class responseimpl;
class cgiimpl;

//! Known HTTP request types

typedef enum {
	OPTIONS,
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	TRACE,
	CONNECT
} method_t;

//! An HTTP request

//! This is a subclass of \ref messageimpl "messageimpl" that defines
//! additional methods for HTTP messages that represent requests.

class requestimpl : public messageimpl {

public:

	//! A structure that maps a method identifier to its name

	struct method_list_t {
		method_t method;	//!< Method
		const char *name;	//!< Printable name
	};

	static const method_list_t method_list[];
	//!< Lookup table for mapping a method to its name

	//! Whether this request should be formatted as a proxy request

	bool proxyFormat;

private:
	//! Method

	method_t method;

	//! The URI

	uriimpl uri;

	//! HTTP version number.

	httpver_t httpver;

public:
	//! The "Authorization" header.
	static const char www_authorization[];

	//! The "Proxy-Autherization" header.
	static const char proxy_authorization[];

	//! Constructor - construct an empty request object

	//! You get to fill in on the details, after construction,
	//! using the various set() methods. The request method gets
	//! initialized to GET, and the HTTP version to 1.1, unless
	//! you say otherwise. No headers.
	requestimpl() noexcept;

	//! Constructor - construct from a CGI environment

	//! The headers get copied from the CGI request, the method
	//! and the URI is also taken from the CGI request. The HTTP
	//! version is set to 1.1. The Host: header gets erased (it
	//! gets merged into the URI).

	requestimpl(const cgiimpl &cgi);

	//! Constructor

	//! This constructor takes the method, and the URI, then
	//! some arbitrary list of header names and header values,
	//! and ending with an optional HTTP version:
	//!
	//! \code
	//! INSERT_LIBX_NAMESPACE::http::requestimpl req(INSERT_LIBX_NAMESPACE::http::GET, "http://example.com",
	//!                 "Accept", "text/plain",
	//!                 "Accept-Language", "en",
	//!                 INSERT_LIBX_NAMESPACE::http::http11);
	//! \endcode
	//!
	template<typename... Args_t>
	requestimpl(method_t methodArg,
		    const uriimpl &uriArg,
		    Args_t && ...args)
		: proxyFormat(false), method(methodArg),
		  uri(uriArg), httpver(httpver_t::http11)
	{
		uri.fixSchemePort();
		init_request_params(std::forward<Args_t>(args)...);
	}

private:
	//! End of initialization parameters
	void init_request_params()
	{
	}

	//! End of initialization parameters with an HTTP version
	void init_request_params(httpver_t httpverArg)
	{
		httpver=httpverArg;
	}

	//! Initialize headers in the HTTP request

	template<typename... Args_t>
	void init_request_params(const std::string &name,
				 const std::string &value,
				 Args_t && ...args)
	{
		append(name, value);
		init_request_params(std::forward<Args_t>(args)...);
	}

public:

	//! Destructor
	~requestimpl();

	//! Set the proxy flag

	//! This flag affects the output of toString().
	//!
	void setProxyFormat(bool flag) noexcept
	{
		proxyFormat=flag;
	}

	//! Retrieve the request message type

	method_t getMethod() const noexcept
	{
		return method;
	}

	//! Retrieve the request URI

	const uriimpl &getURI() const noexcept
	{
		return uri;
	}

	//! Retrieve the request URI

	uriimpl &getURI() noexcept
	{
		return uri;
	}

	//! Retrieve the HTTP version

	httpver_t getVersion() const noexcept
	{
		return httpver;
	}

	//! Return any cookies the client sent.

	void getCookies(//! Any cookies are placed here
			std::map<std::string, std::string> &cookies) const;

	//! Set the request message type

	void setMethod(method_t methodArg)
	{
		method=methodArg;
	}

	//! Set the request URI

	void setURI(const uriimpl &uriArg) noexcept
	{
		uri=uriArg;
	}

	//! Set the HTTP version

	void setVersion(httpver_t versionArg) noexcept
	{
		httpver=versionArg;
	}

	//! Parse an HTTP/1.1 request message out of some input sequence

	//! \return The iterator after the terminating blank line has been
	//! read from the sequence.
	//!
	//! A parsing error throws an exception.
	//!
	//! The Host: header gets removed, and if the request-URI is a
	//! relative URI, the uri's authority is set from the host header,
	//! and scheme gets set to 'http'. In this manner, both proxy-format
	//! and non-proxy format request URIs will result in a request message
	//! with an absolute IR>

	template<typename iter_type>
	iter_type parse(//! Beginning input sequence iterator
			iter_type beg_iter,

			//! Ending input sequence iterator
			iter_type end_iter,

			//! Maximum number of headers to retrieve
			size_t maxlimit)
	{
		beg_iter=messageimpl::parse(beg_iter, end_iter, maxlimit);
		parse_start_line();
		return beg_iter;
	}

	//! Write the HTTP/1.1 request message to an output sequence

	//! If the proxy flag is set, the request references an absolute URI.
	//! If the proxy flag is not set, the authority part is dropped from
	//! the URI, and the Host: header gets added.

	template<typename iter_type>
	iter_type toString(//! Output iterator
			   iter_type iter) const
	{
		for (const char *p=methodstr(); *p; p++)
			*iter++ = *p;

		*iter++ = ' ';

		if (method == CONNECT)
		{
			std::string hostport=uri.getAuthority().hostport;

			iter=std::copy(hostport.begin(), hostport.end(), iter);
		}
		else
			iter=uri.toString(iter, !proxyFormat);
		*iter++ = ' ';
		iter=httpver_toString(iter, httpver);
		*iter++ = '\r';
		*iter++ = '\n';

		if (method != CONNECT)
		{
			std::string h(hostheader());
			iter=std::copy(h.begin(), h.end(), iter);
		}

		return messageimpl::toString(iter);
	}

private:
	//! Generate the "Host:" header for this request

	//! \internal
	//!
	std::string hostheader() const;

public:
	//! Determine whether this request has a message body

	//! \return \c true if there is a Content-Length or a Transfer-Encoding
	//! header, \c false otherwise.

	bool hasMessageBody() const;

	//! Determine whether this request should have a message body

	//! A POST and a PUT should have a message body, other methods don't

	bool shouldHaveMessageBody() const;

	//! Determine whether a response to this request has a message body

	//! A response to a HEAD request, or a 1xx, 204, or 304 responses
	//! do not have a message body. All other requests have a message body.

	bool responseHasMessageBody(const responseimpl &resp) const;

	//! Retrieve basic authentication credentials in the request.

	template<typename Functor>
	void get_basic_auth(//! \ref www_authorization "www_authorization" or \ref proxy_authorization "proxy_authorization".

			    const std::string &header,

			    //! Callback functor.

			    //! The callback functor gets invoked for every
			    //! found header. It receives the base64-decoded
			    //! "userid:password" string.

			    Functor &&functor) const
	{
		for (auto headers=this->equal_range(header);
		     headers.first != headers.second; ++headers.first)
		{
			std::string basic_auth;

			if (is_basic_auth(headers.first->second.begin(),
					  headers.first->second.end(),
					  basic_auth))
				functor(basic_auth);
		}
	}

private:
	//! Parse the start line.

	//! \internal
	void parse_start_line();

	//! Helper function for parsing the method string

	static bool parse_method_str(std::string::const_iterator p,
				     size_t plen,
				     const char *str) noexcept;

public:
	//! Parse basic authentication header

	//! Returns \c true if the first word in this header is "basic"

	static bool is_basic_auth(//! Header start
				  std::string::const_iterator b,

				  //! Header end
				  std::string::const_iterator e,

				  //! base64-decoded stuff goes here
				  std::string &basic_auth);

	//! Convert a method_t to a string

	static const char *methodstr(method_t) noexcept;
private:
	//! This message could not be parsed
	void bad_message() override __attribute__((noreturn));

public:
	//! Convert a string to a method_t

	//! Throws an exception if the string is not a valid method_t
	//!
	static method_t methodnum(const std::string &name)
;

	//! Return this request's method_t as a string

	const char *methodstr() const noexcept
	{
		return methodstr(method);
	}

	//! An error has occured read or writing to a temporary file.

	//! This is used by the receiverimpl template
	//!

	static void tmpfile_error() __attribute__((noreturn));
};

#ifndef DOXYGEN
extern template std::istreambuf_iterator<char>
requestimpl::parse(std::istreambuf_iterator<char>,
		   std::istreambuf_iterator<char>, size_t);

extern template std::ostreambuf_iterator<char>
requestimpl::toString(std::ostreambuf_iterator<char>) const;
#endif

#if 0
{
	{
#endif
	}
}
#endif
