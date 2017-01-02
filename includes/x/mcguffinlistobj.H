/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mcguffinlistobj_H
#define x_mcguffinlistobj_H

#include <x/mcguffincontainerobj.H>
#include <x/weakcontainerobj.H>
#include <list>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Implement a weak mcguffin list container object.

//! \internal
//!
//! \see mcguffinlist.

template<typename ref_type, typename Allocator>
class mcguffinlistObj :
	public weakContainerObj<ref_type,
				std::list<typename
					  mcguffincontainerObj<ref_type>
					  ::container_element_t,
					  Allocator>,
				Allocator> {

public:
	//! The superclass

	typedef weakContainerObj<ref_type,
				 std::list<typename
					   mcguffincontainerObj<ref_type>
					   ::container_element_t,
					   Allocator>,
				 Allocator> weak_container_t;

	//! Default constructor
	explicit mcguffinlistObj(const Allocator &a=Allocator()) noexcept
				 : weak_container_t(a)
	{
	}

	//! Default destructor
	~mcguffinlistObj()
	{
	}

	//! ptr<> type in this weak list.

	typedef typename mcguffincontainerObj<ref_type>::ptr_t ptr_t;

	//! Implement push_front()

	//! A mcguffin gets created, and a weak reference to the mcguffin
	//! gets added to the beginning of the weak container list.

	ref<obj> push_front(//! A strong reference to an object
			    const ptr_t &ptrarg)
	{
		if (ptrarg.null())
			return ref<obj>::create();

		auto lock=weak_container_t::rlock::create(this);

		typename weak_container_t::container_t::writelock
			w(lock->container());

		ref<typename weak_container_t::cleanupObj>
			destroyed=ref<typename weak_container_t::cleanupObj>
			::create(this);

		auto mcguffin=ref<mcguffincontainerObj<ref_type>>
				 ::create(ptrarg);

		auto wrapper=mcguffin->create_wrapper();

		wrapper->ondestroy([destroyed]{destroyed->destroyed();});

		destroyed->install(w->emplace(w->begin(), wrapper));

		return mcguffin;
	}

	//! Implement push_back()

	//! A mcguffin gets created, and a weak reference to the mcguffin
	//! gets added to the beginning of the weak container list.

	ref<obj> push_back(const ptr_t &ptrarg)
	{
		if (ptrarg.null())
			return ref<obj>::create();

		auto lock=weak_container_t::rlock::create(this);

		typename weak_container_t::container_t::writelock
			w(lock->container());

		ref<typename weak_container_t::cleanupObj>
			destroyed=ref<typename weak_container_t::cleanupObj>
			::create(this);

		auto mcguffin=ref<mcguffincontainerObj<ref_type>>
				 ::create(ptrarg);

		auto wrapper=mcguffin->create_wrapper();

		wrapper->ondestroy([destroyed]{destroyed->destroyed();});

		destroyed->install(w->emplace(w->end(), wrapper));

		return mcguffin;
	}
};

#if 0
{
#endif
}
#endif
