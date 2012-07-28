/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN
LIBCXX_TEMPLATE_DECL template
class clientimpl<std::ostreambuf_iterator<char>,
		 std::istreambuf_iterator<char> >;

#define LIBCXX_TEMPLATE_CLIENT_INPUT_ITER std::istreambuf_iterator<char>
#include <x/http/clientimpl_to.H>
#undef LIBCXX_TEMPLATE_CLIENT_INPUT_ITER

#define LIBCXX_TEMPLATE_CLIENT_INPUT_ITER std::string::iterator
#include <x/http/clientimpl_to.H>
#undef LIBCXX_TEMPLATE_CLIENT_INPUT_ITER

#define LIBCXX_TEMPLATE_CLIENT_INPUT_ITER std::string::const_iterator
#include <x/http/clientimpl_to.H>
#undef LIBCXX_TEMPLATE_CLIENT_INPUT_ITER
#endif
