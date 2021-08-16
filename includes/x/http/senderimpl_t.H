/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#define LIBCXX_HTTP_SENDER_OUTPUT_ITER std::ostreambuf_iterator<char>
#include <x/http/senderimpl_to.H>
#undef LIBCXX_HTTP_SENDER_OUTPUT_ITER

#define LIBCXX_HTTP_SENDER_OUTPUT_ITER discardoutput
#include <x/http/senderimpl_to.H>
#undef LIBCXX_HTTP_SENDER_OUTPUT_ITER
