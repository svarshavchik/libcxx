/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakunordered_mapfwd_H
#define x_weakunordered_mapfwd_H

#include <x/ref.H>
#include <x/ptr.H>
#include <x/weakptrfwd.H>
#include <unordered_map>
namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename K, typename T, typename M>
class weakmapObj;

template<typename K, typename T, typename H, typename KE, typename Allocator>
class weakunordered_mapBase;

//! A unordered_map of weak references.

//! This template defines a reference-counted object that implements a subset
//! of functionality offered by std::unordered_map.
//!
//! This is similar to \ref weakmap "weakmap", but using a
//! std::unordered_map as the underlying container.

template<typename K, typename T,
	 typename H=std::hash<K>,
	 typename KE=std::equal_to<K>,
	 typename Allocator=std::allocator<std::pair<const K, weakptr<ptr<T> > > > >
using weakunordered_map=ref<weakmapObj<K, ptr<T>,
				       std::unordered_map<K, weakptr<ptr<T>>,
							  H, KE, Allocator>>,
			    weakunordered_mapBase<K, T, H, KE, Allocator> >;

//! A nullable pointer reference to a reference-counted unordered_map weak container.

//! \see weakunordered_map

template<typename K, typename T,
	 typename H=std::hash<K>,
	 typename KE=std::equal_to<K>,
	 typename Allocator=std::allocator<std::pair<const K, weakptr<ptr<T> > > > >
using weakunordered_mapptr=ptr<weakmapObj<K, ptr<T>,
					  std::unordered_map<K, weakptr<ptr<T>>,
							     H, KE, Allocator>>,
			       weakunordered_mapBase<K, T, H, KE, Allocator> >;

#if 0
{
#endif
}
#endif
