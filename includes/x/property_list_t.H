/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN

#define LIBCXX_TEMPLATE_ITER std::wstring::const_iterator
#include <x/property_list_to.H>
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER std::istreambuf_iterator<wchar_t>
#include <x/property_list_to.H>
#undef LIBCXX_TEMPLATE_ITER

#endif
