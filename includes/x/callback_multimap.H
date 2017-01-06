/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_callback_multimap_H
#define x_callback_multimap_H

#include <x/callback_map_or_multimap.H>
#include <x/callback_multimapfwd.H>
#include <x/callback_container_impl.H>
#include <map>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Traits for a \ref callback_containerObj "callback container" based on a \c std::multimap.

//! Inherits from the common
//! \ref callback_map_or_multimap_impl "callback_map_or_multimap_impl"
//! template and implements install(), returning the iterator of the inserted
//! callback.

template<typename signature, typename priority_type>
class callback_multimap_impl
	: public callback_map_or_multimap_impl <
	signature, std::multimap<priority_type, functionref<signature>>>
{
public:
	//! Import a typedef, for convenience.

	typedef std::multimap<priority_type, functionref<signature>> container_t;
	//! Import a typedef, for convenience.

	typedef typename container_t::iterator iterator;

	//! Import a typedef, for convenience.

	typedef typename container_t::key_type key_type;

	//! Install a new callback

	//! The callback gets inserted into a multimap.

	inline iterator install(const functionref<signature> &callback,
				key_type key)
	{
		return this->insert({key, callback});
	}
};

#if 0
{
#endif
}
#endif
