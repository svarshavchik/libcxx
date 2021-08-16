/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventhandlerobj_H
#define x_eventhandlerobj_H

#include <x/obj.H>
#include <x/ptr.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Event handler base class

//! When an event is dispatched to the event factory, the factory invokes
//! this callback for all registered listeners. Derive from this class, and
//! implement the event() method.
//!
//! \note
//! This is a reference-counted object that must be allocated on the heap,
//! and accessed via reference handles.

template<typename argType>
class eventhandlerObj : virtual public obj {

public:
	//! Default constructor
	eventhandlerObj() noexcept {}

	//! Default destructor
	~eventhandlerObj() {}

	//! Dispatch an event

	//! When an event occurs, it is dispatched by invoking this callback
	//! function.
	//!
	//! \note
	//! The event() method gets invoked by whichever thread dispatched
	//! the event, which may not be the same thread that instantiated
	//! this event handler class.

	virtual void event(//! Event argument
			   const argType &arg)=0;
};

#if 0
{
#endif
}
#endif
