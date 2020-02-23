/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN

#define LIBCXX_TEMPLATE_ITER std::string::const_iterator
#include <x/property_list_to.H>
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER std::istreambuf_iterator<char>
#include <x/property_list_to.H>
#undef LIBCXX_TEMPLATE_ITER

#endif
