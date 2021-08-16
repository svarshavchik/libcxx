/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakunordered_mapiterator_H
#define x_weakunordered_mapiterator_H

#include <x/weakmapobj.H>
#include <unordered_map>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Specialization for processing of the return value from std::map::insert().

template<typename K, typename V, typename H, typename KE, typename A>
class weakmapiteratorOps<std::unordered_map<K, V, H, KE, A> > {

	//! Shorthand for the map

	typedef std::unordered_map<K, V, H, KE, A> map_type;

public:
	//! The type returned by std::map's insert() method

	typedef typename std::pair<typename map_type::iterator, bool>
	map_insert_ret_type;

	//! Check if the insert() method succeeded

	static inline bool inserted(const map_insert_ret_type &i) noexcept
	{
		return i.second;
	}

	//! Return the inserted iterator

	static typename map_type::iterator iter(const map_insert_ret_type &i)
		noexcept
	{
		return i.first;
	}
};

#if 0
{
#endif
}
#endif
