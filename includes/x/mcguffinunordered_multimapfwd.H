/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinunordered_multimapfwd_H
#define x_mcguffinunordered_multimapfwd_H

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
class mcguffinunordered_multimapBase;

//! Weak mcguffin unordered multimap container.

//! This is a version of \ref mcguffinmap "INSERT_LIBX_NAMESPACE::mcguffinmap"
//! that uses a \c std::unordered_multimap as the underlying container.

template<typename K, typename ref_type,
	 typename H=std::hash<K>,
	 typename KE=std::equal_to<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using mcguffinunordered_multimap
=ref<mcguffinmapObj<K, ref_type,
		    std::unordered_multimap<K, typename
				       mcguffincontainerObj<ref_type>
				       ::container_element_t, H, KE,
				       Allocator>>,
     mcguffinunordered_multimapBase<K, ref_type, H, KE, Allocator> >;

//! A nullable pointer reference to a \ref mcguffinunordered_multimap "weak mcguffin unordered multimap container".

template<typename K, typename ref_type,
	 typename H=std::hash<K>,
	 typename KE=std::equal_to<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using mcguffinunordered_multimapptr
=ptr<mcguffinmapObj<K, ref_type,
		    std::unordered_multimap<K,
				       typename mcguffincontainerObj<ref_type>
				       ::container_element_t, H, KE,
				       Allocator>>,
     mcguffinunordered_multimapBase<K, ref_type, H, KE, Allocator> >;

//! A reference to a constant \ref mcguffinunordered_multimap "weak mcguffin unordered multimap container".

template<typename K, typename ref_type,
	 typename H=std::hash<K>,
	 typename KE=std::equal_to<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using const_mcguffinunordered_multimap=
	const_ref<mcguffinmapObj<K, ref_type,
				 std::unordered_multimap<K, typename
						    mcguffincontainerObj<ref_type>
						    ::container_element_t,
						    H, KE, Allocator>>,
		  mcguffinunordered_multimapBase<K, ref_type, H, KE, Allocator> >;

//! A nullable pointer reference to a constant \ref mcguffinunordered_multimap "weak mcguffin unordered multimap container".

template<typename K, typename ref_type,
	 typename H=std::hash<K>,
	 typename KE=std::equal_to<K>,
	 typename Allocator=std::allocator
	 <std::pair<const K, typename mcguffincontainerObj<ref_type>
		    ::container_element_t > > >
using const_mcguffinunordered_multimapptr=
	const_ptr<mcguffinmapObj<K, ref_type,
				 std::unordered_multimap<K, typename
						    mcguffincontainerObj
						    <ref_type>
						    ::container_element_t,
						    H, KE,
						    Allocator>>,
		  mcguffinunordered_multimapBase<K, ref_type, H, KE, Allocator> >;

#if 0
{
#endif
}
#endif
