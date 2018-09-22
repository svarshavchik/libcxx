/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_callback_map_or_multimap_H
#define x_callback_map_or_multimap_H

#include <x/callback_containerfwd.H>
#include <x/functionalrefptr.H>
#include <utility>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename signature, typename container_t>
class callback_map_or_multimap_impl;

//! Traits for a \ref callback_containerObj "callback container" based on a map or a multimap.

//! Derives from a \c std::map, or a \c std::multimap
//! implementing a container for callbacks that get invoked in a priority
//! order.
//!
//! dereference() of an iterator simply returns the \c second value, the
//! callback.
//!
//! \ref callback_multimap_impl "callback_multimap_impl" and
//! \ref callback_map_impl "callback_map_impl" implement install().

template<typename signature, typename container_t>
class callback_map_or_multimap_impl : public container_t {

public:

	//! Import a typedef, for convenience.
	typedef typename container_t::iterator iterator;

	//! Dereference an iterator to the container.

	static functionref<signature> dereference(const iterator &iter)
	{
		return iter->second;
	}
};


#if 0
{
#endif
}

#endif
