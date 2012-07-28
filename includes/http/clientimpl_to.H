/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN
LIBCXX_TEMPLATE_DECL template
class clientimpl<std::ostreambuf_iterator<char>,
		 std::istreambuf_iterator<char> >
::sendmsgbody<LIBCXX_TEMPLATE_CLIENT_INPUT_ITER>;

LIBCXX_TEMPLATE_DECL template
bool clientimpl<std::ostreambuf_iterator<char>,
		std::istreambuf_iterator<char> >
::send(requestimpl &,
       LIBCXX_TEMPLATE_CLIENT_INPUT_ITER,
       LIBCXX_TEMPLATE_CLIENT_INPUT_ITER,
       responseimpl &);
#endif
