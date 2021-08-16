/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/receiverimpl.H"
#include "x/http/requestimpl.H"
#include "x/http/responseimpl.H"
#include "x/chrcasecmp.H"

#include "gettext_in.h"

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

void throw_error_state()
{
	throw EXCEPTION(_("Internal error: invalid HTTP message state"));
}

bool receiverimplcommon::get_transfer_encoding(const messageimpl &msg,
					       messageimpl::const_iterator &p)

{
	p=msg.find(messageimpl::transfer_encoding);

	if (p == msg.end() ||
	    chrcasecmp::str_equal_to()(p->second.value(), "identity"))
		return false;

	return true;
}

void receiverimplcommon::is_chunked_encoding(messageimpl::const_iterator p)

{
	if (!chrcasecmp::str_equal_to()(p->second.value(), "chunked"))
		responseimpl::throw_not_implemented();
}

bool receiverimplcommon::has_content_length(const messageimpl &msg,
					    uint64_t &cnt)
{
	messageimpl::const_iterator p=msg.find(messageimpl::content_length);

	if (p == msg.end())
		return false;

	std::istringstream i(p->second.value());

	cnt=0;

	i >> cnt;

	if (i.fail())
		responseimpl::throw_bad_request();
	return true;
}

template class
receiverimplbase<std::istreambuf_iterator<char> >;

template class
receiverimpl<requestimpl, std::istreambuf_iterator<char> >;

template class
receiverimpl<responseimpl, std::istreambuf_iterator<char> >;

template class receiverimplbase<fdinputiter>;

template class receiverimpl<requestimpl, fdinputiter>;

template class receiverimpl<responseimpl, fdinputiter>;

#if 0
{
#endif
}
