/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinmapfwd_H
#define x_mcguffinmapfwd_H

#include <x/ptrfwd.H>
#include <x/weakmapfwd.H>
#include <x/mcguffincontainerfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename K,
	 typename ref_type,
	 typename M>
class mcguffinmapObj;

template<typename K,
	 typename ref_type,
	 typename M,
	 typename C,
	 typename Allocator>
class mcguffinmapBase;

//! Weak mcguffin map container.

//! This is similar to a \ref weakmap "INSERT_LIBX_NAMESPACE::weakmap",
//! except that insert() returns a mcguffin instead of a \c bool value.
//! A null() \c INSERT_LIBX_NAMESPACE::ptr<INSERT_LIBX_NAMESPACE::obj> gets
//! returned if the key's value already exists, otherwise a non-null()
//! mcguffin gets returned, which owns
//! a strong reference on the inserted value.
//!
//! The inserted value gets removed from the list when the mcguffin, not the
//! inserted value, goes out of scope and gets destroyed, even if there are
//! other
//! strong references to the inserted value in scope.
//!
//! Note that just like a \c INSERT_LIBX_NAMESPACE::weakmap, it's possible
//! that a "zombie" map value exists for the key, a value whose mcguffin
//! went out of scope and got destroyed, but there existing iterators to the
//! weak mcguffin map's container remain in scope. The values from the
//! underlying weak map get removed only after the last iterator to the map
//! goes out of scope and gets destroyed.
//!
//! The first template parameter is the map's key.
//! The second template parameter must be a \c INSERT_LIBX_NAMESPACE::ref<class>
//! and an optional third template parameter specifies the key's comparator
//! class.
//!
//! Just like a \ref weakmap "INSERT_LIBX_NAMESPACE::weakmap", the
//! container's
//! \ref mcguffincontainerObj<ref<objClass,baseType>> "value that's referenced by an iterator"
//! implements a
//! \c getptr() method that returns the inserted value, if it still exists
//! in the map. There are also two additional methods:
//!
//! - mcguffin() returns an opaque \c INSERT_LIBX_NAMESPACE::ptr<INSERT_LIBX_NAMESPACE::obj> mcguffin
//! for the inserted value. A null() \c ptr gets returned if the value no
//! longer exists in the container.
//!
//! - erase() removes this value from the container, if it still exists, even
//! if its mcguffin is still in scope, somewhere.
//!
//! So there are really two ways to remove the inserted value from the map:
//! the mcguffin going out of scope and getting destroyed, and invoking
//! erase().
//!
//! All \ref weakmap "INSERT_LIBX_NAMESPACE::weakmap" methods work the same
//! here: insert(), begin(), end(), find(), count(), lower_bound(),
//! upper_bound(), equal_range().
//!
//! find_or_create() should not be used with a
//! \c INSERT_LIBX_NAMESPACE::mcguffinmap as it will throw a null pointer
//! exception if it cannot either retrieve a strong reference for the key,
//! nor create another one due to a zombie value.
//! find_or_create() should be used only with a
//! \ref mcguffinmultimap "INSERT_LIBX_NAMESPACE::weakmcguffinmultimap".

template<typename K, typename ref_type,
	 typename C=std::less<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using mcguffinmap
=ref<mcguffinmapObj<K, ref_type,
			std::map<K, typename
				 mcguffincontainerObj<ref_type>
				 ::container_element_t, C,
				 Allocator>>,
     mcguffinmapBase<K, ref_type,
			 std::map<K, typename mcguffincontainerObj<ref_type>
				  ::container_element_t, C,
				  Allocator>, C, Allocator> >;

//! A nullable pointer reference to a \ref mcguffinmap "weak mcguffin map container".

template<typename K, typename ref_type,
	 typename C=std::less<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using mcguffinmapptr
=ptr<mcguffinmapObj<K, ref_type,
			std::map<K, typename mcguffincontainerObj<ref_type>
				 ::container_element_t, C, Allocator>>,
     mcguffinmapBase<K, ref_type,
			 std::map<K, typename mcguffincontainerObj<ref_type>
				  ::container_element_t, C, Allocator>,
			 C, Allocator> >;

//! A reference to a constant \ref mcguffinmap "weak mcguffin map container".

template<typename K, typename ref_type,
	 typename C=std::less<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using const_mcguffinmap=
	const_ref<mcguffinmapObj<K, ref_type,
				     std::map<K, typename
					      mcguffincontainerObj<ref_type>
					      ::container_element_t, C,
					      Allocator>>,
		  mcguffinmapBase<K, ref_type,
				      std::map<K, typename
					       mcguffincontainerObj<ref_type>
					       ::container_element_t, C,
					       Allocator>, C, Allocator> >;

//! A nullable pointer reference to a constant \ref mcguffinmap "weak mcguffin map container".

template<typename K, typename ref_type,
	 typename C=std::less<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using const_mcguffinmapptr=
	const_ptr<mcguffinmapObj<K, ref_type,
				     std::map<K, typename
					      mcguffincontainerObj
					      <ref_type>
					      ::container_element_t, C,
					      Allocator>>,
		  mcguffinmapBase<K, ref_type,
				      std::map<K, typename
					       mcguffincontainerObj<ref_type>
					       ::container_element_t, C,
					       Allocator>, C, Allocator> >;

#if 0
{
#endif
}
#endif
