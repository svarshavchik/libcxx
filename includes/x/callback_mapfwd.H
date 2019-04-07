/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_callback_mapfwd_H
#define x_callback_mapfwd_H

#include <x/ptrfwd.H>
#include <x/callback_containerfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A prioritized list of callbacks.

//! Implements a \ref callback_containerObj "container for callbacks"
//! that's based on a
//! \ref callback_list_impl "std::map".
//!
//! Like \ref callback_multimap "callback_multimap", but using
//! \c std::map for container, so there can only be one installed callback
//! with a given priority. Attempting to install() another one with the
//! same priority does nothing (check installed(), to determine this).

template<typename priority_type, typename signature>
using callback_map=
	ref<callback_containerObj<signature,
				  callback_map_impl<signature,
						    priority_type>>,
	    callback_container_implBase<signature,
					callback_map_impl<signature,
							  priority_type>>>;


#if 0
{
#endif
}

#endif
