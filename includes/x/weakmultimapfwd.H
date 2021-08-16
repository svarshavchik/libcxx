/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakmultimapfwd_H
#define x_weakmultimapfwd_H

#include <x/ref.H>
#include <x/ptr.H>
#include <x/weakptrfwd.H>

#include <map>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif


template<typename K, typename T, typename M>
class weakmapObj;

template<typename K, typename T,
	 typename C,
	 typename Allocator> class weakmultimapBase;

//! A multimap of weak references.

//! This template defines a reference-counted object that implements a subset
//! of functionality offered by std::multimap.
//!
//! This template is similar to weakmap, except that it uses a std::multimap
//! container. Subsequently, the only time insert() returns \c false is if the
//! passed reference is null, since insert() always succeeds in a
//! \c std::multimap.
//!
//! See \ref weakmap "INSERT_LIBX_NAMESPACE::weakmap" for more information.
//! Except for the presence of duplicate keys, all other aspects of the
//! weak multimap are the same. Note that a weak map's insert() always
//! return true.

template<typename K, typename T,
	 typename C=std::less<void>,
	 typename Allocator=std::allocator<std::pair<const K, weakptr<ptr<T> > > > >
using weakmultimap=ref<weakmapObj<K, ptr<T>,
				  std::multimap<K, weakptr<ptr<T> >,
						C, Allocator>>,
		       weakmultimapBase<K, T, C, Allocator> >;

//! A nullable pointer reference to a reference-counted multimap weak container.

//! \see weakmultimap
//!

template<typename K, typename T,
	 typename C=std::less<void>,
	 typename Allocator=std::allocator<std::pair<const K, weakptr<ptr<T> > > > >
using weakmultimapptr=ptr<weakmapObj<K, ptr<T>,
				     std::multimap<K, weakptr<ptr<T> >,
						   C, Allocator>>,
			  weakmultimapBase<K, T, C, Allocator> >;

#if 0
{
#endif
}
#endif
