/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#define LIBCXX_TEMPLATE_CHAR char
#include <x/csv_tc.H>
#undef LIBCXX_TEMPLATE_CHAR

#define LIBCXX_TEMPLATE_CHAR wchar_t
#include <x/csv_tc.H>
#undef LIBCXX_TEMPLATE_CHAR
