/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventfactoryobj_H
#define x_eventfactoryobj_H

#include <x/obj.H>
#include <x/ref.H>
#include <x/eventfactoryfwd.H>
#include <x/eventhandlerobj.H>
#include <x/eventregistration.H>

#include <x/sipobj.H>
#include <x/exception.H>

#include <map>
#include <iostream>
#include <cstdlib>

#include <x/namespace.h>


namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Debugging macro

//! \internal
//!
#ifndef LIBCXX_EVENTFACTORY_IMPL_DEBUGHOOK1
#define LIBCXX_EVENTFACTORY_IMPL_DEBUGHOOK1() do { } while(0)
#endif

//! Debugging macro

//! \internal
//!
#ifndef LIBCXX_EVENTFACTORY_IMPL_DEBUGHOOK2
#define LIBCXX_EVENTFACTORY_IMPL_DEBUGHOOK2() do { } while(0)
#endif

//! Debugging macro

//! \internal
//!
#ifndef LIBCXX_EVENTFACTORY_EVENT_DEBUGHOOK
#define LIBCXX_EVENTFACTORY_EVENT_DEBUGHOOK() do { } while(0)
#endif

//! Event factory implementation

template<typename keyType, typename argType, class Compare, class Allocator>
class eventfactoryObj : virtual public obj {

	//! Internal multimap value

	typedef ref<eventhandlerObj<argType> > entry_type;

	//! Internal multimap type
	typedef std::multimap<keyType, entry_type, Compare, Allocator> map_t;

	//! The protected container for the handler map

	typedef sipobj<map_t> map_container_t;

	//! All registered event handlers.
	mutable map_container_t handlers;

	//! Implementation of a registered event handle

	//! This internal implementation does not need to be exposed internally.
	//! A reference to this object gets returned by registerHandler().
	//! This object stores a reference to its factory, and the iterator
	//! value of a registered event handler. When the last reference to this
	//! object goes out of scope, its destructor removes the registered
	//! event handler from the factory.

	class eventregistrationImplObj : public eventregistrationObj {

	public:
		//! Type of an iterator value of registered event handlers.
		typedef typename map_t::iterator iter_t;

		//! Iterator value of the registered event handler.
		iter_t iter;

		//! The factory type where the handler gets registered.
		typedef
		ref<eventfactoryObj<keyType, argType, Compare, Allocator>
		    > factory_t;

		//! The factory where the handler gets registered.
		factory_t factory;

		//! Default constructor
		eventregistrationImplObj(const factory_t &factoryArg,
					 const iter_t &iterArg)
			: iter(iterArg), factory(factoryArg)
		{
		}

		//! Destructor

		//! The registered event handler gets removed from the factory.

		~eventregistrationImplObj()
		{
			eventfactoryObj<keyType, argType, Compare,
					Allocator> &f= *factory;

			LIBCXX_EVENTFACTORY_IMPL_DEBUGHOOK1();

			typename map_container_t::writelock lock(f.handlers);

			lock->erase(iter);
			LIBCXX_EVENTFACTORY_IMPL_DEBUGHOOK2();
		}
	};

public:
	//! Default constructor

	eventfactoryObj() noexcept {}

	//! Register a new event handler

	//! \return the handle for the registered event handler. When the
	//! handle goes out of scope, the handler gets deregistered.

	eventregistration registerHandler( //! Handler's key
					  const keyType &key,

					  //! The handler itself
					  const entry_type &handler)

	{
		typename map_container_t::writelock lock(handlers);

		typename eventregistrationImplObj::iter_t iter=
			lock->insert(std::make_pair(key, handler));

		try {
			return ref<eventregistrationImplObj>::create
				(typename
				 eventregistrationImplObj::factory_t(this),
				 iter);

		} catch (...)
		{
			lock->erase(iter);
			throw;
		}
	}

	//! Default destructor

	~eventfactoryObj()
	{
	}

	//! Register a new event

	//! The event argument gets dispatched to all handlers thar are
	//! registered for this event.

	void event(//! Event handler key
		   const keyType &key,
		   //! Event argument
		   const argType &arg) const
	{
		std::list<entry_type> handler_list;

		// We don't want to hold a lock when invoking event handlers.
		// Make a copy of the event handlers while holding a lock,
		// then release the lock and use the list to fire off events

		{
			typename map_container_t::readlock lock(handlers);

			LIBCXX_EVENTFACTORY_EVENT_DEBUGHOOK();

			for (auto iter=lock->equal_range(key);
			     iter.first != iter.second; ++iter.first)
			{
				handler_list.push_back(iter.first->second);
			}
		}

		for (auto &handler:handler_list)
		{
			handler->event(arg);
		}
	}

	//! Register multiple events.

	//! This single method dispatches multiple events, at once.
	//!

	void event(//! A list of key+argument events.

		   const std::list< std::pair<keyType, argType> > &event_list)
		const
	{
		for (auto &single_event:event_list)
		{
			event(single_event.first, single_event.second);
		}
	}

	//! Register an event for all handlers

	void event(//! Event argument
		   const argType &arg) const
	{
		std::list<entry_type> all_handlers;

		{
			typename map_container_t::readlock lock(handlers);

			for (auto handler: *lock)
				all_handlers.push_back(handler.second);
		}

		while (!all_handlers.empty())
		{
			all_handlers.front()->event(arg);
			all_handlers.pop_front();
		}
	}
};

#if 0
{
#endif
}

#endif
