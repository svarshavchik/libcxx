/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventfactory_H
#define x_eventfactory_H

#include <x/eventfactoryobj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! An event factory

//! This template class defines an "event factory". The factory registers
//! individual "event" handlers. The first template parameter specifies the
//! class that represents an event key. Each event is identified by a key.
//! An event handler gets registered for a single key.
//!
//! When an event gets reported to the factory, all registered handlers for
//! the event's key are notified. This is done by invoking each handler's
//! callback method. With each event there's an associated "argument".
//! The second template parameter specifies the argument's class. Each
//! registered event handler's callback method receives the event argument.
//!
//! \par Creating a new event factory
//!
//! \code
//! typedef eventfactory<std::string, int> myfactory_t;
//!
//! myfactory_t myfactory(myfactory_t::create());
//! \endcode
//!
//! This example creates an event factory for events that are keyed by a
//! \c std::string, with \c int argument values. Loosely restated, the events
//! are named by a \c std::string and have a single \c int value associated
//! with them.
//!
//! Events can be keyed by any class, not just a \c std::string. The class
//! should have a comparison operation defined. Alternatively, an optional
//! third template parameter specifies a comparison class. An optional fourth
//! template parameter specifies an allocator for factory's internal multimap
//! (rarely needed).
//!
//! \ref eventfactory "eventfactory" is a \ref ref "ref" to a
//! reference-counted object. The \ref eventfactory "eventfactory" handle
//! may be freely passed around. The factory gets destroyed when its
//! last reference goes out of scope, and all event handlers are removed from
//! the factory.
//!
//! \par Reporting an event
//!
//! \code
//! myfactory->event("fileavailable", 4);
//! \endcode
//!
//! The \link eventfactoryObj::event() event() \endlink method
//! reports a "fileavailable" event, with an argument of \c 4.
//! If the factory has any registered handlers for "fileavailable" events, their
//! callback methods get invoked, and receive \c 4 as the event argument.
//!
//! For convenience, there's an overloaded event() method that takes a list
//! of event keys and arguments, and reports all of them, at once.
//!
//! There's also an event() method that takes only a single argument, and
//! sents the argument to all registered event handlers.
//!
//! \par Creating event handlers
//!
//! \code
//! class myEventHandlerObj : public eventhandlerObj<int> {
//!
//! public:
//!    myEventHandlerObj() noexcept {}
//!    ~myEventHandlerObj() {}
//!    void event(const int &eventArg);
//! };
//!
//! typedef ref<myEventHandlerObj> myEventHandler;
//!
//! myEventHandler evh(myEventHandler::create());
//!
//! \endcode
//!
//! An event handler class gets defined by subclassing the eventhandlerObj
//! template class. The template parameter specifies the event argument class,
//! which must be the same argument class as the factory's argument class.
//!
//! An individual event handler gets created by instantiating a reference to the
//! handler class. Event handlers are also reference-counted objects.
//!
//! The event handler defines an \c event() method with one argument,
//! a constant reference to the event type. \c event()
//! takes the appropriate action
//! upon receiving the event. Its \c event() method can get invoked
//! concurrently by different threads. The event handler is responsible for
//! making its \c event() method thread safe.
//!
//! \c event() methods should be relatively quick. They should not
//! take an excessively
//! long time to complete. The event factory class is thread safe. A tardy
//! \c event() method may result in lengthy waiting by other threads.
//!
//! \par Registering event handlers
//!
//! \code
//! eventregistration regevent=myfactory->registerHandler("fileavailable", evh);
//! \endcode
//!
//! After it's created, the event handler must be registered with the factory.
//! The above example registers the handler for "fileavailable" events.
//! The handler's event handler now gets invoked every time a "fileavailable"
//! event gets reported.
//!
//! registerHandler() returns a handle. This handle represents the
//! registered event handler. The handle is a reference to an opaque
//! reference-counted object, which may be freely passed around. When
//! the last reference to the handle object goes out of scope, the
//! event handle gets unregistered, removed from the event factory, and
//! will no longer receive events.
//!
//! \note
//! References to event handlers are maintained while an event is in the
//! process of being reported. After the event registration handler goes out
//! of scope, it is still possible for the event handler to be invoked from
//! another thread that's already in the middle of reporting an event.
//!
//! \par Multiple threads
//!
//! The event factory is thread safe. Note, however,
//! that it's possible for multiple threads to report the same event, with
//! the event handler's callback method invoked by multiple threads at the
//! same time, once for each individual event. The event handler's thread
//! safety is its own responsibility.

template<typename keyType, typename argType,
	 class Compare = std::less<keyType>,
	 class Allocator = std::allocator<std::pair<const keyType,
						    ref<eventhandlerObj<argType> >
						    > > >
using eventfactory=ref< eventfactoryObj<keyType, argType, Compare, Allocator> >;

//! A nullable pointer to an event factory

//! \see eventfactory
//!
template<typename keyType, typename argType,
	 class Compare = std::less<keyType>,
	 class Allocator = std::allocator<std::pair<const keyType,
						    ref<eventhandlerObj<argType> >
						    > > >
using eventfactoryptr=ptr<eventfactoryObj<keyType, argType, Compare, Allocator>
			   >;

#if 0
{
#endif
}

#endif
