/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_responseimpl_H
#define x_http_responseimpl_H

#include <x/http/messageimplfwd.H>
#include <x/chrcasecmp.H>
#include <x/http/clientauthimplfwd.H>
#include <exception>
#include <algorithm>
#include <sstream>
#include <cstring>
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

	//! Note that on the server side, \ref response_exception puts
	//! quoted parameter values into scheme_parameters_t. This is because
	//! some parameters, according to RFC 2617, must always be quoted.
	typedef std::multimap<std::string, std::string,
			      chrcasecmp::str_less> scheme_parameters_t;

	//! Default constructor
	responseimpl() noexcept;

	//! Common constructor
	responseimpl(int statuscodeArg, const std::string &reasonphraseArg,
		     httpver_t httpverArg=httpver_t::http11);

	//! Common constructor
	responseimpl(int statuscodeArg, const char *reasonphraseArg,
		     httpver_t httpverArg=httpver_t::http11);

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

public:

	//! The "WWW-Authenticate" header.
	static const char www_authenticate[];

	//! The "Proxy-Authenticate" header.
	static const char proxy_authenticate[];

	//! The authentication-info header
	static const char authentication_info[];

	//! The proxy-authentication-info header
	static const char proxy_authentication_info[];

	//! The www-authenticate code
	static const int www_authenticate_code=401;

	//! The proxy-authenticate code
	static const int proxy_authenticate_code=407;

	//! A parameter in an authentication challenge

	struct auth_param {

		//! Parameter name
		const char *name;

		//! Whether the parameter value must be always quoted
		bool always_quoted;

		//! Associate container operator

		bool operator<(const auth_param &o) const
		{
			return strcmp(name, o.name) < 0;
		}

		//! Quote the value, accordingly
		std::string quote_value(const std::string &value) const;
	};

	//! Known authentication parameters

	static const auth_param auth_realm; // Authentication realm

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
		__attribute__((noreturn));

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
		__attribute__((noreturn));

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

	//! Check if this is a 401 response
	bool www_authentication_required() const
	{
		return statuscode == www_authenticate_code;
	}

	//! Check if this is a 407 response
	bool proxy_authentication_required() const
	{
		return statuscode == proxy_authenticate_code;
	}

	//! List value populated by process_challenge_header()

	struct challenge_info {

		//! Authentication scheme
		auth scheme;

		//! Authentication realm
		std::string realm;

		//! Additional scheme parameters
		scheme_parameters_t params;

		//! Constructor
		challenge_info();

		//! Constructor
		challenge_info(auth schemeArg,
			       const std::string &realmArg,
			       const scheme_parameters_t &paramsArg);

		//! Add a parsed challenge to a list of challenges.

		//! This is for challenges(), which parses the challenge
		//! header into here. All the parameters get parsed into
		//! params. add() verifies that there is a "realm"
		//! parameter, which gets removed from params and placed into
		//! realm, before this gets added to the list. If there was
		//! a parsing error, scheme is unknown. If either the realm
		//! is missing, or the scheme is unknown, this challenge_info
		//! does not get added to the list.

		void add(std::list<challenge_info> &list);
	};

	//! Parse a challenge header

	//! Fills the list with parsed authentication scheme. Unknown
	//! scheme are quietly ignored.

	static void
	challenges(//! Header value beginning iterator
		   std::string::const_iterator b,

		   //! Header value ending iterator
		   std::string::const_iterator e,

		   //! Populated list
		   std::list<challenge_info> &list);

	//! Extract authentication challenges from this response

	//! If www_authentication_required() or proxy_authentication_required(),
	//! retrieve the challenges from the response. The parameter is a
	//! callback functor that gets invoked with three parameters:
	//! an authentication scheme,
	//! the authentication realm, and a scheme_parameters_t, containing
	//! the authentication scheme's parameters.
	//!
	//! The callback does not get invoked for unknown schemes.

	template<typename callback_type>
	void challenges(callback_type &&callback) const
	{
		const char *p;

		if (www_authentication_required())
			p=www_authenticate;
		else if (proxy_authentication_required())
			p=proxy_authenticate;
		else return;

		headersbase::process(p,
				     [&]
				     (std::string::const_iterator b,
				      std::string::const_iterator e)
				     {
					     std::list<challenge_info> list;

					     challenges(b, e, list);

					     for (const auto &c:list)
						     callback(c.scheme,
							      c.realm,
							      c.params);
				     });
	}

	//! Parse the Authentication-Info header into scheme_parameters_t

	//! Returns true if the header is present.

	bool get_authentication_info(//! Either authentication_info or proxy_authentication_info
				     const char *header,

				     //! params gets cleared, then populated
				     //! with the parsed header contents
				     scheme_parameters_t &params) const;

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

	//! Add a WWW-Authenticate header

	template<typename ...Args>
	void add_authenticate(Args && ...args)
	{
		add_authorization(www_authenticate,
				  std::forward<Args>(args)...); 
	}

	//! Add a Proxy-Authenticate header

	template<typename ...Args>
	void add_proxy_authenticate(Args && ...args)
	{
		add_authorization(proxy_authenticate,
				  std::forward<Args>(args)...); 
	}

private:
	//! Parse the start line.

	//! \internal
	void parse_start_line();

	//! Construct a WWW-Authenticate or a Proxy-Authenticate header

	//! \internal

	template<typename ...Args>
	void add_authorization(const char *header,
			       httpver_t httpverArg,
			       Args && ...args)
	{
		setVersion(httpverArg);
		add_authorization_params(header,
					 std::forward<Args>(args)...);
	}

	//! Construct a WWW-Authenticate or a Proxy-Authenticate header

	//! \internal

	template<typename ...Args>
	void add_authorization(const char *header,
			       auth first_scheme,
			       Args && ...args)
	{
		add_authorization_params(header, first_scheme,
					 std::forward<Args>(args)...);
	}

	//! Construct a WWW-Authenticate or a Proxy-Authenticate header

	//! \internal

	void add_authorization(const char *header,
			       const std::list<responseimpl::challenge_info>
			       &challenges)
	{
		add_authorization_params(header, challenges);
	}

	//! Construct a WWW-Authenticate or a Proxy-Authenticate header

	//! Skipped the optional httpver_t parameter.
	//!
	//! \internal

	template<typename ...Args>
	void add_authorization_params(const char *header,
				      auth first_scheme,
				      Args && ...args)
	{
		add_auth_set(header, first_scheme,
			     std::forward<Args>(args)...);
	}

	//! Construct a WWW-Authenticate or a Proxy-Authenticate header

	//! Skipped the optional httpver_t parameter.
	//!
	//! \internal

	void add_authorization_params(const char *header,
				      const std::list<responseimpl
				      ::challenge_info> &challenges)
	{
		add_auth_set(header, challenges);
	}

	//! Finish constructing a WWW-Authenticate or Proxy-Authenticate header

	//! \internal

	template<typename ...Args>
	void add_auth_set(const char *header,
			  auth firstArg,
			  Args && ...args)
	{
		std::list<std::string> word_list;

		build_auth_challenge(word_list, firstArg,
				std::forward<Args>(args)...);
		save_auth_header(header, word_list);
	}

	//! Finish constructing a WWW-Authenticate or Proxy-Authenticate header

	//! \internal

	template<typename ...Args>
	void add_auth_set(const char *header,
			  const responseimpl::challenge_info &firstArg,
			  Args && ...args)
	{
		std::list<std::string> word_list;

		build_auth_challenge(word_list, firstArg,
				     std::forward<Args>(args)...);
		save_auth_header(header, word_list);
	}


	//! Finish constructing a WWW-Authenticate or Proxy-Authenticate header

	//! \internal

	void add_auth_set(const char *header,
			  const std::list<responseimpl::challenge_info>
			  &challenges);

	//! Finished collecting authentication parameters, emit the header

	void save_auth_header(const char *header,
			      const std::list<std::string> &word_list);

	//! Formatting of authentication challenges

	//! No more challanges follow.
	//!
	void build_auth_challenge(std::list<std::string> &word_list)
	{
	}

	//! Start collecting the next challenge from the variadic parameters.

	template<typename ...Args>
	void build_auth_challenge(std::list<std::string> &word_list,
				  auth next_scheme,
				  const std::string &realm,
				  Args && ...args)
	{
		responseimpl::challenge_info challenge;

		challenge.scheme=next_scheme;
		challenge.realm=realm;

		collect_auth_challenge(word_list, challenge,
				 std::forward<Args>(args)...);
	}

	//! The next challenge is already prepared for us.

	template<typename ...Args>
	void build_auth_challenge(std::list<std::string> &word_list,
				  const responseimpl::challenge_info
				  &nextChallenge,
				  Args && ...args)
	{
		add_auth_challenge(word_list, nextChallenge);
		build_auth_challenge(word_list, std::forward<Args>(args)...);
	}

	//! Another parameter for the current challenge.

	template<typename ...Args>
	void collect_auth_challenge(std::list<std::string> &word_list,
				    responseimpl::challenge_info &challenge,
				    const responseimpl::auth_param &param,
				    const std::string &value,
				    Args && ...args)
	{
		challenge.params
			.insert(std::make_pair(param.name, param.quote_value(value)));

		collect_auth_challenge(word_list, challenge,
				 std::forward<Args>(args)...);
	}

	//! End of current challenge, start of next one.

	template<typename ...Args>
	void collect_auth_challenge(std::list<std::string> &word_list,
				    responseimpl::challenge_info &challenge,
				    auth nextArg,
				    Args && ...args)
	{
		add_auth_challenge(word_list, challenge);
		build_auth_challenge(word_list, nextArg,
				 std::forward<Args>(args)...);
	}

	//! End of current challenge, start of next one.

	template<typename ...Args>
	void collect_auth_challenge(std::list<std::string> &word_list,
				    responseimpl::challenge_info &challenge,
				    const responseimpl::challenge_info &nextArg,
				    Args && ...args)
	{
		add_auth_challenge(word_list, challenge);
		build_auth_challenge(word_list, nextArg,
				 std::forward<Args>(args)...);
	}

	//! Last challenge

	void collect_auth_challenge(std::list<std::string> &word_list,
				    responseimpl::challenge_info &challenge)
	{
		add_auth_challenge(word_list, challenge);
	}

	//! Save a complete challenge, collected from variadic parameters.
	void add_auth_challenge(std::list<std::string> &word_list,
				const responseimpl::challenge_info &challenge);

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
