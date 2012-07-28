/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventregistration_H
#define x_eventregistration_H

#include <x/obj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A handle for a registered event handler

//! When the last reference to this object goes out of scope, the corresponding
//! event handler is deregistered from its event factory.

class eventregistrationObj : virtual public obj {

public:
	eventregistrationObj() noexcept;
	~eventregistrationObj() noexcept;
};

//! Reference to a registered event handler

//! This handle is returned by
//! \link eventfactoryObj::registerHandler() registerHandler()\endlink.
//! When the last reference to the handle goes out of scope, the corresponding
//! event handler is removed from the event factory.

typedef ref<eventregistrationObj> eventregistration;

//! A pointer to a registered event handler

//! \see eventregistration
typedef ptr<eventregistrationObj> eventregistrationptr;

#if 0
{
#endif
}
#endif
