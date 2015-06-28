/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/senderimpl.H"
#include "x/property_properties.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

property::value<size_t>
senderimpl_encode::chunksize(LIBCXX_NAMESPACE_STR
			     "::http::chunksize", 8192);

void senderimpl_encode::expected_message_body()
{
	throw EXCEPTION(_("Internal error: HTTP message body not provided for a message that should have one"));
}

void senderimpl_encode::unexpected_message_body()
{
	throw EXCEPTION(_("Internal error: HTTP message body provided for a message that shouldn't have one"));
}

senderimpl_encode::wait_continue::wait_continue()
{
}

senderimpl_encode::wait_continue::~wait_continue() noexcept
{
}

bool senderimpl_encode::wait_continue::operator()()
{
	return true;
}

#define LIBCXX_TEMPLATE_DECL
#include "x/http/senderimpl_t.H"
#undef LIBCXX_TEMPLATE_DECL

#if 0
{
	{
#endif
	}
}
