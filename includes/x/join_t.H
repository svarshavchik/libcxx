/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#define LIBCXX_CHAR_TYPE char
#include <x/join_to.H>
#undef LIBCXX_CHAR_TYPE

#define LIBCXX_CHAR_TYPE wchar_t
#include <x/join_to.H>
#undef LIBCXX_CHAR_TYPE
