/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakunordered_multimapiterator_H
#define x_weakunordered_multimapiterator_H

#include <x/weakmapobj.H>
#include <map>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Specialization for processing of the return value from std::unordered_multimap::insert().

template<typename K, typename V, typename H, typename KE, typename A>
class weakmapiteratorOps<std::unordered_multimap<K, V, H, KE, A> > {

	//! Shorthand for the map

	typedef std::unordered_multimap<K, V, H, KE, A> map_type;

public:
	//! The type returned by std::map's insert() method

	typedef typename map_type::iterator map_insert_ret_type;

	//! Check if the insert() method succeeded

	static inline bool inserted(const map_insert_ret_type &i) noexcept
	{
		return true;
	}

	//! Return the inserted iterator

	static typename map_type::iterator iter(const map_insert_ret_type &i)
		noexcept
	{
		return i;
	}
};

//! Helper class for implementing correct weakmapObj::find_or_create() semantics

//! Specialization for a weakunordered_multimap
//! \internal

template<typename K, typename V, typename H, typename KE, typename A>
class handle_find_or_create<std::unordered_multimap<K, V, H, KE, A>> {

 public:
	//! Ho-hum.

	template<typename objClass, typename baseType,
		 typename key_type,
		 typename ptr_type>
	static inline ref<objClass, baseType>
		insert_into_map(ptr_type *m,
				const key_type &key,
				const ref<objClass, baseType> &value)
	{
		m->insert(key, value);
		return value;
	}

	//! Ho-hum.

	template<typename objClass, typename baseType,
		 typename key_type,
		 typename ptr_type>
		static std::pair<ref<obj>, ref<objClass, baseType>>
		insert_into_mcguffin(ptr_type *m,
				     const key_type &key,
				     const ref<objClass, baseType> &value)
	{
		return std::pair<ref<obj>, ref<objClass, baseType>>
			(m->insert(key, value), value);
	}
};

#if 0
{
#endif
}
#endif
