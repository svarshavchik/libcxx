/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinunordered_map_H
#define x_mcguffinunordered_map_H

#include <x/mcguffinunordered_mapfwd.H>
#include <x/mcguffinmapobj.H>
#include <x/weakmap.H>
#include <x/weakunordered_mapiterator.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for \ref mcguffinunordered_map "weak mcguffin map container" objects.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::mcguffinunordered_map<key,ref_type>::base

template<typename K,
	 typename ref_type,
	 typename H,
	 typename KE,
	 typename Allocator>
class mcguffinunordered_mapBase : public ptrref_base {

public:

	//! The underlying reference-counted object.
	typedef mcguffinmapObj<K, ref_type,
			       std::unordered_map<K, typename
						  mcguffincontainerObj<ref_type>
						  ::container_element_t,
						  H, KE,
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
