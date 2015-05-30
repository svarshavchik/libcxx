/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weaklist_H
#define x_weaklist_H

#include <x/weaklistfwd.H>
#include <x/weaklistobj.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for a weak list container pointer or reference

//! Refer to this class as \c customweaktype::base or \c customweaktypeptr::base
//!
template<typename T, typename Allocator>
class weaklistBase : public ptrrefBase {

public:

	//! The type representing a reference to the container

	typedef typename Allocator::reference       reference;

	//! The type representing a constant reference to the container

	typedef typename Allocator::const_reference const_reference;

	//! The type representing the size of the container

	typedef typename weaklistObj<ptr<T>, Allocator>::size_type size_type;

	//! The type representing the iterator of the container

	typedef typename weaklistObj<ptr<T>, Allocator>::iterator iterator;

	//! The type representing the contents of the container

	typedef typename weaklistObj<ptr<T>, Allocator>::value_type value_type;

	//! The type of the allocator for weak references
	typedef Allocator allocator_type;
};

#if 0
{
#endif
}
#endif