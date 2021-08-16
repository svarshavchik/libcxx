/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_callback_multimapfwd_H
#define x_callback_multimapfwd_H

#include <x/ptrfwd.H>
#include <x/callback_containerfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A prioritized list of callbacks.

//! Implements a \ref callback_containerObj "container for callbacks"
//! that's based on a
//! \ref callback_list_impl "std::list".
//!
//! The first template parameter is the priority key type.
//! The second template parameter specifies the callback signatures.
//!
//! install() tales an additional parameter, the priority key for the new
//! callback.
//!
//! The callbacks get invoked in priority order, from low-priority values to
//! higher (strict weak ordering).
//!
//! The invocation order of callbacks with the same priority is not specified.

template<typename priority_type, typename signature>
using callback_multimap=
	ref<callback_containerObj<signature,
				  callback_multimap_impl<signature,
							 priority_type>>,
	    callback_container_implBase<signature,
					callback_multimap_impl<signature,
							       priority_type>>>;

#if 0
{
#endif
}

#endif
