/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_callback_map_H
#define x_callback_map_H

#include <x/callback_map_or_multimap.H>
#include <x/callback_mapfwd.H>
#include <x/callback_container_impl.H>
#include <map>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename signature, typename priority_type>
class callback_map_impl;

//! Traits for a \ref callback_containerObj "callback container" based on a \c std::map.

//! Inherits from the common
//! \ref callback_map_or_multimap_impl "callback_map_or_multimap_impl"
//! template and implements install(), returning the iterator of the inserted
//! callback, or end() if the key already exists in the map.

//! Implements \ref callback_containerObj "callbacks" with the list of callbacks
//! invoked one at a time. Inherits from \c std::list, storing them in
//! a plain list, and they get invoked one at a time.
//!
//! A simple traits class. Each traits class is a template. Its parameter
//! is the callback's signature.
//!
//! The traits template should inherit from a suitable container. The
//! install() method's first parameter is the new \ref functionref "callback"
//! to be added to the container. Any additional parameters
//! to the \ref callback_containerObj "callback list's install() method" get
//! forwarded to the template class.
//!
//! dereference() receives an iterator to the container, and it should return
//! a copy of the dereferenced \c functionref that was originally
//! install()ed.
//!
//! The static \c invoke() method receives the \c functionref as its first
//! parameter. It should invoke the functionref, forwarding to it any
//! additional parameters, then return a \c bool.
//!
//! A return value of \c true indicates that any remaining callbacks should
//! continue to get invoked. \c false stops the invocation of the callbacks.

template<typename signature, typename priority_type>
class callback_map_impl
	: public callback_map_or_multimap_impl <
	signature, std::map<priority_type, functionref<signature>>>
{
public:
	//! Import a typedef, for convenience.

	typedef std::map<priority_type, functionref<signature>> container_t;

	//! Import a typedef, for convenience.

	typedef typename container_t::iterator iterator;

	//! Import a typedef, for convenience.

	typedef typename container_t::key_type key_type;

	//! Install a new callback

	//! The callback gets inserted into a map.

	inline iterator install(const functionref<signature> &callback,
				key_type key)
	{
		auto ret=this->insert({key, callback});

		return ret.second ? ret.first:this->end();
	}
};

#if 0
{
#endif
}
#endif
