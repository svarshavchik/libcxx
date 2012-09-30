/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_responseimpl_H
#define x_http_responseimpl_H

#include <x/http/messageimplfwd.H>
#include <x/chrcasecmp.H>
#include <exception>
#include <algorithm>
#include <sstream>
#include <map>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

class response_exception;

//! HTTP informational responses are in the 1xx range.
static const int resp_informational = 1;

//! HTTP success responses are in the 2xx range.
static const int resp_success = 2;

//! HTTP redirection responses are in the 3xx range.
static const int resp_redirected = 3;

//! HTTP client error responses are in the 4xx range.
static const int resp_client_error = 4;

//! HTTP server error responses are in the 5xx range.
static const int resp_server_error = 5;

//! An HTTP response

//! This is a subclass of \ref messageimpl "messageimpl" that defines
//! additional methods for HTTP messages that represent responses.

class responseimpl : public messageimpl {

	//! HTTP version

	httpver_t httpver;

	//! Three digit status code

	int statuscode;

	//! Reason phrase.
	std::string reasonphrase;

public:
	//! The container for authentication challenge scheme's parameters

	typedef std::multimap<std::string, std::string,
			      chrcasecmp::str_less> scheme_parameters_t;

private:
	//! Internal class used by challenges()

	//! \internal
	//!
	class authschemeparser : private tokenhdrparser {

		//! Most recent word returned by tokenhdrparser
		std::string current_word;

		//! Most recent returned value from tokenhdrparser::operator().
		char current_char;

	public:
		//! Constructor
		authschemeparser(//! Name of header to parse
				 const char *headername,

				 //! Headers
				 const headersbase &headers);

		//! Destructor
		~authschemeparser() noexcept;

		//! Parsed scheme parameters.
		scheme_parameters_t scheme_parameters;

		//! The extracted realm
		std::string realm;

		//! Parse the next authentication scheme.

		//! \return the next authentication scheme's name. An empty
		//! string indicates no more authentication scheme. The
		//! authentication parameters are placed into scheme_parameters.

		std::string operator()(void);
	};

public:

	//! Default constructor
	responseimpl() noexcept;

	//! Common constructor
	responseimpl(int statuscodeArg, const std::string &reasonphraseArg,
		     httpver_t httpverArg=http11);

	//! Common constructor
	responseimpl(int statuscodeArg, const char *reasonphraseArg,
		     httpver_t httpverArg=http11);

	//! Destructor
	~responseimpl() noexcept;

	//! Retrieve the HTTP version

	httpver_t getVersion() const noexcept
	{
		return httpver;
	}

	//! Get the status code

	int getStatusCode() const noexcept
	{
		return statuscode;
	}

	//! Get the status code class

	int getStatusCodeClass() const noexcept
	{
		return statuscode / 100;
	}

	//! Get the reason phrase

	const std::string &getReasonPhrase() const noexcept
	{
		return reasonphrase;
	}

	//! Set the HTTP version
	void setVersion(httpver_t versionArg) noexcept
	{
		httpver=versionArg;
	}

	//! Set the status code
	void setStatusCode(int status);

	//! Set the reason phrase
	void setReasonPhrase(const std::string &reasonphrase);

	//! Set current date
	void setCurrentDate();

	//! Set all the non-caching headers.

	//! Sets Cache-Control: no-cache and Pragma: no-cache directives
	void setNoCache();

private:

	//! Text of a standard HTTP error message
	static const char msg_400[];

	//! Text of a standard HTTP error message
	static const char msg_401[];

	//! Text of a standard HTTP error message
	static const char msg_402[];

	//! Text of a standard HTTP error message
	static const char msg_403[];

	//! Text of a standard HTTP error message
	static const char msg_404[];

	//! Text of a standard HTTP error message
	static const char msg_405[];

	//! Text of a standard HTTP error message
	static const char msg_406[];

	//! Text of a standard HTTP error message
	static const char msg_407[];

	//! Text of a standard HTTP error message
	static const char msg_408[];

	//! Text of a standard HTTP error message
	static const char msg_409[];

	//! Text of a standard HTTP error message
	static const char msg_410[];

	//! Text of a standard HTTP error message
	static const char msg_411[];

	//! Text of a standard HTTP error message
	static const char msg_412[];

	//! Text of a standard HTTP error message
	static const char msg_413[];

	//! Text of a standard HTTP error message
	static const char msg_414[];

	//! Text of a standard HTTP error message
	static const char msg_415[];

	//! Text of a standard HTTP error message
	static const char msg_416[];

	//! Text of a standard HTTP error message
	static const char msg_417[];

	//! Text of a standard HTTP error message
	static const char msg_500[];

	//! Text of a standard HTTP error message
	static const char msg_501[];

	//! Text of a standard HTTP error message
	static const char msg_502[];

	//! Text of a standard HTTP error message
	static const char msg_503[];

	//! Text of a standard HTTP error message
	static const char msg_504[];

	//! Text of a standard HTTP error message
	static const char msg_505[];
	//! This message could not be parsed
	void bad_message() __attribute__((noreturn));

	//! Common template for throw_unauthorized_exception() and proxy_authentication_required

	//! Constructs an exception, from its variadic parameters, and throws
	//! it.
	//! \internal

	template<typename ...Args>
	static void throwAuthenticateException(int code,
					       const char *msg,
					       const char *header,
					       auth first_scheme,
					       Args && ...args)
		__attribute__((noreturn));

	//! Common template for throw_unauthorized_exception() and proxy_authentication_required

	//! \overload
	//! \internal
	template<typename ...Args>
	static void throwAuthenticateException(int code,
					       const char *msg,
					       const char *header,
					       httpver_t httpverArg,
					       auth first_scheme,
					       Args && ...args)
		__attribute__((noreturn));

public:

	//! Throw an HTTP error %exception

	static void throwResponseException(int err_code, const char *msg)
		__attribute__((noreturn));

	//! Throw an HTTP error %exception

	static void throwResponseException(int err_code, const std::string &msg)
		__attribute__((noreturn));


	//! Throw an HTTP-related %exception
	static void throw_bad_request() __attribute__((noreturn))
	{
		throwResponseException(400, msg_400); 
	}

	//! Throw an HTTP-related %exception
	template<typename ...Args>
	static void throw_unauthorized(Args && ...args)
	{
		throwAuthenticateException(www_authenticate_code, msg_401,
					   www_authenticate,
					   std::forward<Args>(args)...); 
	}

	//! Throw an HTTP-related %exception
	static void throw_payment_required() __attribute__((noreturn))
	{
		throwResponseException(402, msg_402); 
	}

	//! Throw an HTTP-related %exception
	static void throw_forbidden() __attribute__((noreturn))
	{
		throwResponseException(403, msg_403); 
	}

	//! Throw an HTTP-related %exception
	static void throw_not_found() __attribute__((noreturn))
	{
		throwResponseException(404, msg_404); 
	}

	//! Throw an HTTP-related %exception
	static void throw_method_not_allowed() __attribute__((noreturn))
	{
		throwResponseException(405, msg_405); 
	}

	//! Throw an HTTP-related %exception
	static void throw_not_acceptable() __attribute__((noreturn))
	{
		throwResponseException(406, msg_406); 
	}

	//! Throw an HTTP-related %exception
	template<typename ...Args>
	static void throw_proxy_authentication_required(Args && ...args)
	{
		throwAuthenticateException(proxy_authenticate_code, msg_407,
					   proxy_authenticate,
					   std::forward<Args>(args)...); 
	}

	//! Throw an HTTP-related %exception
	static void throw_request_timeout() __attribute__((noreturn))
	{
		throwResponseException(408, msg_408); 
	}

	//! Throw an HTTP-related %exception
	static void throw_conflict() __attribute__((noreturn))
	{
		throwResponseException(409, msg_409); 
	}

	//! Throw an HTTP-related %exception
	static void throw_gone() __attribute__((noreturn))
	{
		throwResponseException(410, msg_410); 
	}

	//! Throw an HTTP-related %exception
	static void throw_length_required() __attribute__((noreturn))
	{
		throwResponseException(411, msg_411); 
	}

	//! Throw an HTTP-related %exception
	static void throw_precondition_failed() __attribute__((noreturn))
	{
		throwResponseException(412, msg_412); 
	}

	//! Throw an HTTP-related %exception
	static void throw_request_entity_too_large() __attribute__((noreturn))
	{
		throwResponseException(413, msg_413); 
	}

	//! Throw an HTTP-related %exception
	static void throw_request_uri_too_long() __attribute__((noreturn))
	{
		throwResponseException(414, msg_414); 
	}

	//! Throw an HTTP-related %exception
	static void throw_unsupported_media_type() __attribute__((noreturn))
	{
		throwResponseException(415, msg_415); 
	}

	//! Throw an HTTP-related %exception
	static void throw_requested_range_not_satisfiable() __attribute__((noreturn))
	{
		throwResponseException(416, msg_416); 
	}

	//! Throw an HTTP-related %exception
	static void throw_expectation_failed() __attribute__((noreturn))
	{
		throwResponseException(417, msg_417); 
	}

	//! Throw an HTTP-related %exception
	static void throw_internal_server_error() __attribute__((noreturn))
	{
		throwResponseException(500, msg_500); 
	}

	//! Throw an HTTP-related %exception
	static void throw_not_implemented() __attribute__((noreturn))
	{
		throwResponseException(501, msg_501); 
	}

	//! Throw an HTTP-related %exception
	static void throw_bad_gateway() __attribute__((noreturn))
	{
		throwResponseException(502, msg_502); 
	}

	//! Throw an HTTP-related %exception
	static void throw_service_unavailable() __attribute__((noreturn))
	{
		throwResponseException(503, msg_503); 
	}

	//! Throw an HTTP-related %exception
	static void throw_gateway_timeout() __attribute__((noreturn))
	{
		throwResponseException(504, msg_504); 
	}

	//! Throw an HTTP-related %exception
	static void throw_http_version_not_supported() __attribute__((noreturn))
	{
		throwResponseException(505, msg_505); 
	}

	//! The "WWW-Authenticate" header.
	static const char www_authenticate[];

	//! The "Proxy-Authenticate" header.
	static const char proxy_authenticate[];

	//! Check if this is a 401 response
	bool www_authentication_required() const
	{
		return statuscode == 401;
	}

	//! Check if this is a 407 response
	bool proxy_authentication_required() const
	{
		return statuscode == 407;
	}

	//! Extract authentication challenges from this response

	//! If www_authentication_required() or proxy_authentication_required(),
	//! retrieve the challenges from the response. The parameter is a
	//! callback functor that gets invoked with three parameters:
	//! an authentication scheme, converted to lowercase,
	//! the authentication realm, and a scheme_parameters_t, containing
	//! the authentication scheme's parameters.

	template<typename callback_type>
	void challenges(callback_type &&callback) const
	{
		const char *p;

		if (www_authentication_required())
			p=www_authenticate;
		else if (proxy_authentication_required())
			p=proxy_authenticate;
		else return;

		authschemeparser parser(p, *this);

		auto scheme=parser();

		while (!scheme.empty())
		{
			callback(scheme, parser.realm,
				 parser.scheme_parameters);

			scheme=parser();
		}
	}

	//! Parse an HTTP/1.1 response message out of some input sequence

	//! \return The iterator after the terminating blank line has been
	//! read from the sequence.
	//!
	//! A parsing error throws an exception.

	template<typename iter_type>
	iter_type parse(//! Beginning input sequence iterator
			iter_type beg_iter,

			//! Ending input sequence iterator
			iter_type end_iter,

			//! Maximum number of lines in the header
			size_t maxlimit)
	{
		beg_iter=messageimpl::parse(beg_iter, end_iter, maxlimit);
		parse_start_line();
		return beg_iter;
	}

	//! Write the HTTP/1.1 response message to an output sequence

	template<typename iter_type>
	iter_type toString(//! Output iterator
			   iter_type iter) const
	{
		iter=httpver_toString(iter, httpver);

		*iter++ = ' ';

		{
			std::ostringstream o;

			o << statuscode;

			std::string s(o.str());

			iter=std::copy(s.begin(), s.end(), iter);
		}

		*iter++=' ';
		iter=std::copy(reasonphrase.begin(), reasonphrase.end(), iter);
		*iter++ = '\r';
		*iter++ = '\n';

		return messageimpl::toString(iter);
	}
private:
	//! Parse the start line.

	//! \internal
	void parse_start_line();
};

#ifndef DOXYGEN
extern template std::istreambuf_iterator<char>
responseimpl::parse(std::istreambuf_iterator<char>,
		    std::istreambuf_iterator<char>, size_t);

extern template std::ostreambuf_iterator<char>
responseimpl::toString(std::ostreambuf_iterator<char>) const;
#endif

#if 0
{
	{
#endif
	}
}
#endif
