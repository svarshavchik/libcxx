/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "http/exception.H"
#include "http/serverimpl.H"
#include "xml/escape.H"
#include "gettext_in.h"
#include <sstream>

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

response_exception::response_exception(int statuscodeArg,
				       const std::string &reasonphraseArg)
	: statuscode(statuscodeArg), reasonphrase(reasonphraseArg)
{
	(*this) << statuscode << ' ' << reasonphrase;

	done();
}

response_exception::~response_exception() noexcept
{
}

std::string response_exception::body() const
{
	std::string reasonstresc=xml::escapestr(reasonphrase);

	return "<html><head><title>" + reasonstresc + "</title></head>"
		"<body><h1>" + reasonstresc + "</h1></body></html>\n";
}
#if 0
{
	{
#endif
	}
}
