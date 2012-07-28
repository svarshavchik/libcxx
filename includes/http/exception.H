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

//! Instantiate a responseimpl and throw it.
//! When a reponseimpl gets caught, invoke toString() to format
//! an HTTP response message.

class response_exception : public exception {

	//! Saved status code
	int statuscode;

	//! Saved reason phrase
	std::string reasonphrase;
public:

	using exception::operator->;
	using exception::operator*;

	//! Constructor
	response_exception(int statuscodeArg,
			   const std::string &reasonphraseArg);

	//! Destructor
	~response_exception() noexcept;

	//! Retrieve the exception code

	int getStatusCode() const noexcept { return statuscode; }

	//! Retrieve the error message
	std::string getReasonPhrase() const noexcept { return reasonphrase; }

	//! Return the HTML body for this exception

	std::string body() const;
};

#if 0
{
	{
#endif
	}
}
#endif
