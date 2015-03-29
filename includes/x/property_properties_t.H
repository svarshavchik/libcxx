/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef DOXYGEN

#define LIBCXX_TEMPLATE_ITER std::wstring::iterator
#include <x/property_properties_to.H>
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER std::wstring::const_iterator
#include <x/property_properties_to.H>
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER std::string::iterator
#include <x/property_properties_to.H>
#undef LIBCXX_TEMPLATE_ITER

#define LIBCXX_TEMPLATE_ITER std::string::const_iterator
#include <x/property_properties_to.H>
#undef LIBCXX_TEMPLATE_ITER


#endif
