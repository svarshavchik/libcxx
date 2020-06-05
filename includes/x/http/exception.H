/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_exception_H
#define x_http_exception_H

#include <exception>
#include <x/http/responseimpl.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

//! A thrown %exception that can formatted as an HTTP message.

//! Instantiate a response_exception and throw it.
//! When a response_exception gets caught, use it as a responseimpl, also
//! invoke body() to get a short, brief body to use for the response.

class response_exception : public exception, public responseimpl {

public:

	using exception::operator->;
	using exception::operator*;

	//! Constructor
	response_exception(int statuscodeArg,
			   const std::string &reasonphraseArg,
			   httpver_t httpverArg=httpver_t::http11);

	//! Destructor
	~response_exception();

	//! The /small/ body.

	std::string body;

	//! Create a canned body string

	void set_canned_body_str();

	//! Create a canned body string, with a little bit more stuff.

	void set_canned_body_str(const std::string &extra);
};

#ifndef DOXYGEN_FIXME
template<typename ...Args>
void responseimpl::throw_proxy_authentication_required(Args && ...args)
{
	response_exception resp(proxy_authenticate_code, msg_407);

	resp.add_proxy_authenticate(std::forward<Args>(args)...);

	throw resp;
}

template<typename ...Args>
void responseimpl::throw_unauthorized(Args && ...args)
{
	response_exception resp(www_authenticate_code, msg_401);

	resp.add_authenticate(std::forward<Args>(args)...);

	throw resp;
}
#endif

#if 0
{
#endif
}
#endif
