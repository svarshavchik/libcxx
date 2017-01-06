/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_callback_listfwd_H
#define x_callback_listfwd_H

#include <x/ptrfwd.H>
#include <x/callback_containerfwd.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename signature, typename container_type>
class callback_containerObj;

template<typename signature, typename container_type>
class callback_container_implBase;

//! A list of callbacks.

//! Implements a \ref callback_containerObj "container for callbacks"
//! that's based on a
//! \ref callback_list_impl "std::list".

template<typename signature>
using callback_list=
	ref<callback_containerObj<signature,
			     callback_list_impl<signature>>,
	    callback_container_implBase<signature,
			      callback_list_impl<signature>>>;


#if 0
{
#endif
}

#endif
