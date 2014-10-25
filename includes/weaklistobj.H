/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weaklistobj_H
#define x_weaklistobj_H

#include <x/weakcontainerobj.H>
#include <list>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Weak reference list container implementation

//! This subclass of weakContainerObj implements the container as a
//! std::list, and provides the push_front() and push_back() methods.
//!
//! The first template parameter must be a ptr to an object. The second
//! parameter is an STL allocator, that's used to allocate weakptr<T> objects
//! that get added to the list.

template<typename T, typename Allocator>
class weaklistObj : public weakContainerObj<T,
					    std::list<weakptr<T>, Allocator>,
					    Allocator> {

public:
	//! The superclass

	typedef weakContainerObj<T, std::list<weakptr<T>, Allocator>,
				 Allocator> weak_container_t;

	//! Default constructor
	explicit weaklistObj(const Allocator &a=Allocator()) noexcept
		: weak_container_t(a)
	{
	}

	//! Default destructor
	~weaklistObj() noexcept
	{
	}

	//! Implement push_front()

	//! A weak reference to the object gets added to the beginning
	//! of the list.

	void push_front(//! A strong reference to an object
			const T &ptrarg)
	{
		if (ptrarg.null())
			return;

		auto lock=weak_container_t::rlock::create(this);

		typename weak_container_t::container_t::writelock
			w(lock->container());

		ref<typename weak_container_t::cleanupObj>
			destroyed=ref<typename weak_container_t::cleanupObj>
			::create(this);

		ptrarg->ondestroy([destroyed]{destroyed->destroyed();});

		destroyed->install(w->emplace(w->begin(), ptrarg));
	}

	//! Implement push_back()

	//! A weak reference to the object gets added to the beginning
	//! of the list.

	void push_back(const T &ptrarg)
	{
		if (ptrarg.null())
			return;

		auto lock=weak_container_t::rlock::create(this);

		typename weak_container_t::container_t::writelock
			w(lock->container());

		ref<typename weak_container_t::cleanupObj>
			destroyed=ref<typename weak_container_t::cleanupObj>
			::create(this);

		ptrarg->ondestroy([destroyed]
				  {
					  destroyed->destroyed();
				  });

		destroyed->install(w->emplace(w->end(), ptrarg));
	}
};

#if 0
{
#endif
}
#endif
