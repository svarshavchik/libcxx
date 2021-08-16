/*
** Copyright 2014-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinlistfwd_H
#define x_mcguffinlistfwd_H

#include <x/ptrfwd.H>
#include <x/mcguffincontainerfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename ref_type, typename Allocator>
class mcguffinlistObj;

template<typename ref_type, typename Allocator>
class mcguffinlistBase;

//! Weak mcguffin list container.

//! This is similar to a \ref weaklist "INSERT_LIBX_NAMESPACE::weaklist",
//! except that push_front() and push_back() returns a mcguffin that owns
//! a strong reference on the inserted value.
//!
//! The value gets removed from the list when the mcguffin, not the inserted
//! value, goes out of scope and gets destroyed, even if there are other
//! strong references to the inserted value in scope.
//!
//! The first template parameter must be a \c INSERT_LIBX_NAMESPACE::ref<class>.
//!
//! Just like a \ref weaklist "INSERT_LIBX_NAMESPACE::weaklist", the
//! container's
//! \ref mcguffincontainerObj<ref<objClass,baseType>> "value that's referenced by an iterator"
//! implements a
//! \c getptr() method that returns the inserted value, if it still exists
//! in the list. There are also two additional methods:
//!
//! - mcguffin() returns an opaque \c INSERT_LIBX_NAMESPACE::ptr<INSERT_LIBX_NAMESPACE::obj> mcguffin
//! for the inserted value. A null() \c ptr gets returned if the value no
//! longer exists in the container.
//!
//! - erase() removes this value from the container, if it still exists, even
//! if its mcguffin is still in scope, somewhere.
//!
//! So there are really two ways to remove the inserted value from the list:
//! the mcguffin going out of scope and getting destroyed, and invoking
//! erase().

template<typename ref_type,
	 typename Allocator=std::allocator<typename
					   mcguffincontainerObj<ref_type>
					   ::container_element_t> >
using mcguffinlist=ref<mcguffinlistObj<ref_type, Allocator>,
			   mcguffinlistBase<ref_type, Allocator>>;

//! A nullable pointer reference to a \ref mcguffinlist "weak mcguffin list container".
template<typename ref_type,
	 typename Allocator=std::allocator<typename
					   mcguffincontainerObj<ref_type>
					   ::container_element_t> >
using mcguffinlistptr=ptr<mcguffinlistObj<ref_type, Allocator>,
			      mcguffinlistBase<ref_type, Allocator>>;


//! A reference to a constant \ref mcguffinlist "weak mcguffin list container".

template<typename ref_type,
	 typename Allocator=std::allocator<typename
					   mcguffincontainerObj<ref_type>
					   ::container_element_t> >
using const_mcguffinlist=
	const_ref<mcguffinlistObj<ref_type, Allocator>,
		  mcguffinlistBase<ref_type, Allocator>>;

//! A nullable pointer reference to a constant \ref mcguffinlist "weak mcguffin list container".

template<typename ref_type,
	 typename Allocator=std::allocator<typename
					   mcguffincontainerObj<ref_type>
					   ::container_element_t> >
using const_mcguffinlistptr=
	const_ptr<mcguffinlistObj<ref_type, Allocator>,
		  mcguffinlistBase<ref_type, Allocator>>;

#if 0
{
#endif
}
#endif
