/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinmap_H
#define x_mcguffinmap_H

#include <x/mcguffinmapfwd.H>
#include <x/mcguffinmapobj.H>
#include <x/weakmap.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for \ref mcguffinmap "weak mcguffin map container" objects.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::mcguffinmap<key,ref_type>::base

template<typename K,
	 typename ref_type,
	 typename M,
	 typename C,
	 typename Allocator>
class mcguffinmapBase : public ptrrefBase {

public:

	//! The underlying reference-counted object.
	typedef mcguffinmapObj<K, ref_type, C,
				   std::map<K, typename
					    mcguffincontainerObj<ref_type>
					    ::container_element_t,
					    C,
					    Allocator>,
				   Allocator> obj_type;

	//! The type representing a reference to the container

	typedef typename Allocator::reference reference;

	//! The type representing a constant reference to the container

	typedef typename Allocator::const_reference const_reference;

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