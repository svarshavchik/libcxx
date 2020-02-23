/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakmapiterator_H
#define x_weakmapiterator_H

#include <x/weakmapobj.H>
#include <map>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Specialization for processing of the return value from std::map::insert().

template<typename K, typename V, typename C, typename A>
class weakmapiteratorOps<std::map<K, V, C, A> > {

	//! Shorthand for the map

	typedef std::map<K, V, C, A> map_type;

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
