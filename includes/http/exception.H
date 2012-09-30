/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_exception_H
#define x_http_exception_H

#include <exception>
#include <x/http/responseimpl.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace http {
#if 0
	};
};
#endif

//! A thrown %exception that can formatted as an HTTP message.

//! Instantiate a response_exception and throw it.
//! When a response_exception gets caught, use it as a responseimpl, also
//! invoke body() to get a short, brief body to use for the response.

class response_exception : public exception, public responseimpl {

	//! End of variadic challenge parameters.

	void buildChallenges(const char *header)
	{
	}

	//! The next challenge is already prepackaged

	template<typename ...Args>
	void buildChallenges(const char *header,
			     auth next_scheme,
			     const scheme_parameters_t &next_params,
			     Args && ...args)
	{
		addChallenge(header, next_scheme, next_params);
		buildChallenges(header, std::forward<Args>(args)...);
	}

	//! Start collecting the next challenge from the variadic parameters.

	template<typename ...Args>
	void buildChallenges(const char *header,
			     auth next_scheme,
			     const std::string &realm,
			     Args && ...args)
	{
		scheme_parameters_t next_params;

		next_params.insert(std::make_pair("realm", realm));

		collectChallenge(header, next_scheme, next_params,
				 std::forward<Args>(args)...);
	}

	//! End of the last challenge

	void collectChallenge(const char *header,
			      auth current_scheme,
			      scheme_parameters_t &current_params)
	{
		addChallenge(header, current_scheme, current_params);
	}

	//! Still collecting the parameters for the next challenge.

	template<typename ...Args>
	void collectChallenge(const char *header,
			      auth current_scheme,
			      scheme_parameters_t &current_params,
			      const std::string &name,
			      const std::string &value,
			      Args && ...args)
	{
		current_params.insert(std::make_pair(name, value));
		collectChallenge(header, current_scheme, current_params,
				 std::forward<Args>(args)...);
	}

	//! Reached current challenge, there's more.

	template<typename ...Args>
	void collectChallenge(const char *header,
			      auth current_scheme,
			      scheme_parameters_t &current_params,
			      auth next_scheme,
			      Args && ...args)
	{
		addChallenge(header, current_scheme, current_params);
		buildChallenges(header, next_scheme,
				std::forward<Args>(args)...);
	}

	//! Save a complete challenge, collected from variadic parameters.
	void addChallenge(const char *header,
			  auth scheme,
			  const scheme_parameters_t &params);
public:

	using exception::operator->;
	using exception::operator*;

	friend class responseimpl;

	//! Constructor
	response_exception(int statuscodeArg,
			   const std::string &reasonphraseArg,
			   httpver_t httpverArg=http11);

	//! Destructor
	~response_exception() noexcept;

	//! Return the HTML body for this exception

	std::string body() const;
};

template<typename ...Args>
void responseimpl::throwAuthenticateException(int code,
					      const char *msg,
					      const char *header,
					      auth first_scheme,
					      Args && ...args)
{
	response_exception resp(code, msg);

	resp.buildChallenges(header, first_scheme, std::forward<Args>(args)...);

	throw resp;
}

#if 0
{
	{
#endif
	}
}
#endif
