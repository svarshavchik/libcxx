/*
** Copyright 2014-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinlist_H
#define x_mcguffinlist_H

#include <x/ref.H>
#include <x/mcguffinlistfwd.H>
#include <x/mcguffinlistobj.H>
#include <x/weakptr.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for \ref mcguffinlist "weak mcguffin list container" objects.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::mcguffinlist<ref_type>::base

template<typename ref_type, typename Allocator>
class mcguffinlistBase : public ptrref_base {

public:

	//! The type representing the size of the container

	typedef typename mcguffinlistObj<ref_type, Allocator>::size_type size_type;

	//! The type representing the iterator of the container

	typedef typename mcguffinlistObj<ref_type, Allocator>::iterator iterator;

	//! The type representing the contents of the container

	typedef typename mcguffinlistObj<ref_type, Allocator>::value_type value_type;

	//! The type of the allocator for weak references
	typedef Allocator allocator_type;
};

#if 0
{
#endif
}
#endif
