/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakmultimap_H
#define x_weakmultimap_H

#include <x/weakmultimapfwd.H>
#include <x/weakmapobj.H>
#include <x/weakmultimapiterator.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for a weak map container pointer or reference

//! Refer to this class as \c customweaktype::base or \c customweaktypeptr::base
//!

template<typename K, typename T, typename C, typename Allocator>
class weakmultimapBase : public ptrref_base {

public:

	//! The underlying reference-counted object.
	typedef weakmapObj<K, ptr<T>, std::multimap<K, weakptr<ptr<T> >,
						       C, Allocator>> obj_type;

	//! The type representing a reference to the container

	typedef typename Allocator::reference       reference;

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
