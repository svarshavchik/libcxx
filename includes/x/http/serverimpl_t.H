/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#define LIBCXX_HTTP_SERVER_OUTPUT_ITER std::ostreambuf_iterator<char>
#include <x/http/serverimpl_to.H>
#undef LIBCXX_HTTP_SERVER_OUTPUT_ITER

#define LIBCXX_HTTP_SERVER_OUTPUT_ITER discardoutput
#include <x/http/serverimpl_to.H>
#undef LIBCXX_HTTP_SERVER_OUTPUT_ITER
