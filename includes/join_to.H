/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#define LIBCXX_CONTAINER_TYPE std::list
#include <x/join_to2.H>
#include <x/join_to4.H>
#undef LIBCXX_CONTAINER_TYPE

#define LIBCXX_CONTAINER_TYPE std::vector
#include <x/join_to2.H>
#include <x/join_to4.H>
#undef LIBCXX_CONTAINER_TYPE

#define LIBCXX_CONTAINER_TYPE std::set
#define LIBCXX_NO_NONCONST_ITERATOR
#include <x/join_to2.H>
#undef LIBCXX_NO_NONCONST_ITERATOR
#undef LIBCXX_CONTAINER_TYPE

#define LIBCXX_CONTAINER_TYPE std::multiset
#include <x/join_to4.H>
#undef LIBCXX_CONTAINER_TYPE
