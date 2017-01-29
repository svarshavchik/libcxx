/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinmultimapfwd_H
#define x_mcguffinmultimapfwd_H

#include <x/mcguffinmapfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename K,
	 typename ref_type,
	 typename C,
	 typename Allocator>
class mcguffinmultimapBase;

//! Weak mcguffin multi-map container.

//! A \ref mcguffinmap "weak mcguffin map container" that uses
//! a \c std::multimap.
//!
//! \see mcguffinmap

template<typename K, typename ref_type,
	 typename C=std::less<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using mcguffinmultimap
=ref<mcguffinmapObj<K, ref_type,
			std::multimap<K, typename
				 mcguffincontainerObj<ref_type>
				 ::container_element_t, C>>,
     mcguffinmultimapBase<K, ref_type, C, Allocator> >;

//! A nullable pointer reference to a \ref mcguffinmultimap "weak mcguffin multimap container".

template<typename K, typename ref_type,
	 typename C=std::less<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using mcguffinmultimapptr
=ptr<mcguffinmapObj<K, ref_type,
			std::multimap<K, typename
				      mcguffincontainerObj<ref_type>
				      ::container_element_t, C, Allocator>>,
     mcguffinmultimapBase<K, ref_type, C, Allocator> >;

//! A reference to a constant \ref mcguffinmultimap "weak mcguffin multimap container".

template<typename K, typename ref_type,
	 typename C=std::less<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using const_mcguffinmultimap=
	const_ref<mcguffinmapObj
		  <K, ref_type,
		   std::multimap<K, typename
				 mcguffincontainerObj<ref_type>
				 ::container_element_t, C, Allocator>>,
		  mcguffinmultimapBase
		  <K, ref_type, C, Allocator> >;

//! A nullable pointer reference to a constant \ref mcguffinmultimap "weak mcguffin multimap container".

template<typename K, typename ref_type,
	 typename C=std::less<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using const_mcguffinmultimapptr=
	const_ptr<mcguffinmapObj<K, ref_type,
				     std::multimap<K, typename
						   mcguffincontainerObj
						   <ref_type>
						   ::container_element_t, C,
						   Allocator>>,
		  mcguffinmultimapBase
		  <K, ref_type, C, Allocator> >;
#if 0
{
#endif
}
#endif
