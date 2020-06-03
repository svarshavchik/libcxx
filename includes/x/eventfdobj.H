/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventfdobj_H
#define x_eventfdobj_H

#include <x/fdobj.H>
#include <x/ptr.H>
#include <x/sys/eventfdsys.h>
#include <x/sys/eventfdt.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A wrapper for an eventfd() file descriptor.

//! This is a subclass of fdObj that instantiates an event file
//! descriptor.

class eventfdObj : public fdObj {

public:
	//! The constructor

	//! The constructor creates a new event file descriptor.
	//!

	eventfdObj();

	//! The destructor

	//! Close the event file descriptor before this object gets
	//! destructed.
	~eventfdObj();

	//! Event notification.

	//! This method announces that one or more events has occured.
	//!
	void event(//! Number of events that has occured.

		   //! Must be at least 1.
		   //!
		   eventfd_t nevents);

	//! Wait for at least one event to occur.

	//! This method waits until at least one event occurs (if none have
	//! occured already.
	//! \return number of events that have occured, may be more than 1.
	//! \note An exception gets thrown if the event file descriptor is
	//! a non-blocking file descriptor, and no events have occured before
	//! invoking this method.
	eventfd_t event();

#include <x/sys/kqueuenb.h>
};

#if 0
{
#endif
}
#endif
