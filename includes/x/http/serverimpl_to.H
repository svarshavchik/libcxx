/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN
LIBCXX_TEMPLATE_DECL template
class serverimpl<std::istreambuf_iterator<char>,
		 LIBCXX_HTTP_SERVER_OUTPUT_ITER>;

#define LIBCXX_HTTP_SERVER_BODY_ITER_TYPE std::istreambuf_iterator<char>
#include <x/http/serverimpl_to2.H>
#undef LIBCXX_HTTP_SERVER_BODY_ITER_TYPE

#define LIBCXX_HTTP_SERVER_BODY_ITER_TYPE std::string::const_iterator
#include <x/http/serverimpl_to2.H>
#undef LIBCXX_HTTP_SERVER_BODY_ITER_TYPE

#ifdef LIBCXX_TEMPLATE_DECL_INST

template<>
template<>
void serverimpl<std::istreambuf_iterator<char>,
		LIBCXX_HTTP_SERVER_OUTPUT_ITER>::
do_send(responseimpl &resp, const requestimpl &req,
	std::string::iterator b, std::string::iterator e)
{
	do_send(resp, req,
		std::string::const_iterator(b),
		std::string::const_iterator(e));
}
#else
template<>
template<>
void serverimpl<std::istreambuf_iterator<char>,
		LIBCXX_HTTP_SERVER_OUTPUT_ITER>::
do_send(responseimpl &, const requestimpl &,
	std::string::iterator, std::string::iterator);
#endif

#endif
