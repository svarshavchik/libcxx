/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_algorithm_H
#define x_algorithm_H

#include <x/functional.H>
#include <stddef.h>
#include <vector>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Sort by indexes.

//! The passed-in vector strictly consists of values 0..sizeof(v)-1. Sorts the
//! vector, calling the passed-in function and passing on each call the indexes
//! in the vector that are getting swapped.
//!
//! An exception gets thrown if it's determined (likely in the middle of
//! sorting) that the passed-in vector fails to meet the requirements.
//!
//! Complexity is O(n).

template<typename functor>
void sort_by(std::vector<size_t> &v, functor &&f)
{
	do_sort_by(v, make_function<void (size_t, size_t)>
		   (std::forward<functor>(f)));
}

//! Type-erased sort_by().

void do_sort_by(std::vector<size_t> &v,
		const function<void (size_t, size_t)> &);


#if 0
{
#endif
}
#endif
