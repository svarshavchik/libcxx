/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakmap_H
#define x_weakmap_H

#include <x/weakmapfwd.H>
#include <x/weakmapobj.H>
#include <x/weakmapiterator.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for a weak map container pointer or reference

//! Refer to this class as \c customweaktype::base or \c customweaktypeptr::base
//!

template<typename K, typename T,
	 typename C,
	 typename Allocator>
class weakmapBase : public ptrref_base {

public:
	//! The underlying reference-counted object.
	typedef weakmapObj<K, ptr<T>, std::map<K, weakptr<ptr<T> >, C,
					       Allocator>> obj_type;

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
