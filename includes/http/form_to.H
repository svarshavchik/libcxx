/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN

#define LIBCXX_TEMPLATE_ITER2 LIBCXX_TEMPLATE_ITER
#include <x/http/form_to2.H>
#undef LIBCXX_TEMPLATE_ITER2

LIBCXX_TEMPLATE_DECL template class limited_iter<LIBCXX_TEMPLATE_ITER>;

#define LIBCXX_TEMPLATE_ITER2 limited_iter<LIBCXX_TEMPLATE_ITER>
#include <x/http/form_to2.H>
#undef LIBCXX_TEMPLATE_ITER2

#endif
