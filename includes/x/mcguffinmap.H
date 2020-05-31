/*
** Copyright 2014-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinmap_H
#define x_mcguffinmap_H

#include <x/mcguffinmapfwd.H>
#include <x/mcguffinmapobj.H>
#include <x/weakptr.H>
#include <x/weakmap.H>
#include <x/weakmapiterator.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for \ref mcguffinmap "weak mcguffin map container" objects.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::mcguffinmap<key,ref_type>::base

template<typename K,
	 typename ref_type,
	 typename C,
	 typename Allocator>
class mcguffinmapBase : public ptrref_base {

public:

	//! The underlying reference-counted object.
	typedef mcguffinmapObj<K, ref_type,
				   std::map<K, typename
					    mcguffincontainerObj<ref_type>
					    ::container_element_t,
					    C,
					    Allocator>> obj_type;

	//! The type representing the size of the container

	typedef typename obj_type::size_type size_type;

	//! The type representing the iterator of the container

	typedef typename obj_type::iterator iterator;

	//! The type representing the contents of the container

	typedef typename obj_type::value_type value_type;

	//! The type of the allocator for weak references
	typedef Allocator allocator_type;
};

#if 0
{
#endif
}
#endif
