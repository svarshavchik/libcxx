/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_callback_list_H
#define x_callback_list_H

#include <x/callback_containerfwd.H>
#include <x/callback_listfwd.H>
#include <x/callback_container_impl.H>
#include <x/functionalrefptr.H>
#include <list>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Traits for a \ref callback_containerObj "callback container" based on a \c std::list.

//! Derives from a \c std::list, implementing a container for callbacks
//! that get invoked in some unspecified order.
//!
//! install() adds the new callback to the front() of the list.
//!
//! dereference() of an iterator simply returns what the iterato points to.

template<typename signature>
class callback_list_impl : public std::list<functionref<signature>> {

public:

	// extern template class won't work with =default (!)

	//! Default constructor
	callback_list_impl() {}

	//! Default destructor
	~callback_list_impl() {}

	//! Import a typedef, for convenience.
	typedef typename std::list<functionref<signature>>::iterator iterator;

	//! Install a new callback

	//! The callback gets added to the list.
	inline iterator install(const functionref<signature> &callback)
	{
		this->push_front(callback);
		return this->begin();
	}

	//! Dereference an iterator to the container.

	static functionref<signature> dereference(const iterator &iter)
	{
		return *iter;
	}
};

extern template class callback_list_impl<void()>;
extern template class callback_container_implObj<void (),
						 callback_list_impl<void ()>>;
extern template class callback_containerObj<void (),
					    callback_list_impl<void ()>>;
#if 0
{
#endif
}

#endif
