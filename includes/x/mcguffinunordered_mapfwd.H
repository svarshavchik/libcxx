/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinunordered_mapfwd_H
#define x_mcguffinunordered_mapfwd_H

#include <x/ptrfwd.H>
#include <x/weakmapfwd.H>
#include <x/mcguffincontainerfwd.H>
#include <unordered_map>

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
	 typename H,
	 typename KE,
	 typename Allocator>
class mcguffinunordered_mapBase;

//! Weak mcguffin unordered map container.

//! This is a version of \ref mcguffinmap "INSERT_LIBX_NAMESPACE::mcguffinmap"
//! that uses a \c std::unordered_map as the underlying container.

template<typename K, typename ref_type,
	 typename H=std::hash<K>,
	 typename KE=std::equal_to<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using mcguffinunordered_map
=ref<mcguffinmapObj<K, ref_type,
		    std::unordered_map<K, typename
				       mcguffincontainerObj<ref_type>
				       ::container_element_t, H, KE,
				       Allocator>>,
     mcguffinunordered_mapBase<K, ref_type, H, KE, Allocator> >;

//! A nullable pointer reference to a \ref mcguffinunordered_map "weak mcguffin unordered map container".

template<typename K, typename ref_type,
	 typename H=std::hash<K>,
	 typename KE=std::equal_to<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using mcguffinunordered_mapptr
=ptr<mcguffinmapObj<K, ref_type,
		    std::unordered_map<K,
				       typename mcguffincontainerObj<ref_type>
				       ::container_element_t, H, KE,
				       Allocator>>,
     mcguffinunordered_mapBase<K, ref_type, H, KE, Allocator> >;

//! A reference to a constant \ref mcguffinunordered_map "weak mcguffin unordered map container".

template<typename K, typename ref_type,
	 typename H=std::hash<K>,
	 typename KE=std::equal_to<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using const_mcguffinunordered_map=
	const_ref<mcguffinmapObj<K, ref_type,
				 std::unordered_map<K, typename
						    mcguffincontainerObj<ref_type>
						    ::container_element_t,
						    H, KE, Allocator>>,
		  mcguffinunordered_mapBase<K, ref_type, H, KE, Allocator> >;

//! A nullable pointer reference to a constant \ref mcguffinunordered_map "weak mcguffin unordered map container".

template<typename K, typename ref_type,
	 typename H=std::hash<K>,
	 typename KE=std::equal_to<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using const_mcguffinunordered_mapptr=
	const_ptr<mcguffinmapObj<K, ref_type,
				 std::unordered_map<K, typename
						    mcguffincontainerObj
						    <ref_type>
						    ::container_element_t,
						    H, KE,
						    Allocator>>,
		  mcguffinunordered_mapBase<K, ref_type, H, KE, Allocator> >;

#if 0
{
#endif
}
#endif
