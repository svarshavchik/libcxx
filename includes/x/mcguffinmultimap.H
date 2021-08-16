/*
** Copyright 2014-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinmultimap_H
#define x_mcguffinmultimap_H

#include <x/mcguffinmap.H>
#include <x/mcguffinmultimapfwd.H>
#include <x/weakmultimapiterator.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for \ref mcguffinmultimap "weak mcguffin multimap container" objects.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::mcguffinmultimap<key,ref_type>::base


template<typename K,
	 typename ref_type,
	 typename C,
	 typename Allocator>
class mcguffinmultimapBase : public ptrref_base {

public:

	//! The underlying reference-counted object.
	typedef mcguffinmapObj<K, ref_type,
				   std::multimap
				   <K, typename
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
