/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/exception.H"
#include "x/http/serverimpl.H"
#include "x/http/clientauthimpl.H"
#include "x/xml/escape.H"
#include "x/tokens.H"
#include "x/join.H"
#include "gettext_in.h"
#include <sstream>
#include <cstring>

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

response_exception::response_exception(int statuscodeArg,
				       const std::string &reasonphraseArg,
				       httpver_t httpverArg)
	: responseimpl(statuscodeArg, reasonphraseArg, httpverArg)
{
	(*this) << getStatusCode() << ' ' << getReasonPhrase();

	setCurrentDate();
	append("Content-Type: text/html");
	done();
}

response_exception::~response_exception() noexcept
{
}

std::string response_exception::body() const
{
	std::string reasonstresc=xml::escapestr(getReasonPhrase());

	return "<html><head><title>" + reasonstresc + "</title></head>"
		"<body><h1>" + reasonstresc + "</h1></body></html>\n";
}

#if 0
{
	{
#endif
	}
}
