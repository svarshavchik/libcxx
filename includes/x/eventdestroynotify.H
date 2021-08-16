/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventdestroynotify_H
#define x_eventdestroynotify_H

#include <x/eventdestroynotifyobj.H>
#include <x/weakptr.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class eventdestroynotifyBase;

//! Notify an event file descriptor when another object gets destroyed

//! \code
//! INSERT_LIBX_NAMESPACE::eventfd eventfd=INSERT_LIBX_NAMESPACE::eventfd::create();
//!
//! INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj> mcguffin=INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj>::create();
//!
//! INSERT_LIBX_NAMESPACE::eventdestroynotify notify=
//!     INSERT_LIBX_NAMESPACE::eventdestroynotify::create(eventfd, mcguffin);
//! \endcode
//!
//! The constructed \c INSERT_LIBX_NAMESPACE::eventdestroynotify gets added
//! as a destructor callback to the mcguffin.
//!
//! When the mcguffin goes out of scope and gets destroyed, the event file
//! descriptor gets notified.

typedef ref<eventdestroynotifyObj, eventdestroynotifyBase> eventdestroynotify;

//! A nullable pointer to an event file descriptor notification object.

//! \see eventdestroynotify

typedef ptr<eventdestroynotifyObj, eventdestroynotifyBase> eventdestroynotifyptr;


//! Refer to this class as \c eventdestroynotify::base or \c eventdestroynotifyptr::base

class eventdestroynotifyBase {

public:

	//! create() always returns \c eventdestroynotify.

	class newobj {

	public:
		//! Create a destructor reference

		static eventdestroynotify
		create(//! Notify this event file descriptor
		       const eventfd &eventfdArg,

		       //! When this object goes out of scope

		       const ref<obj> &objArg);
	};

	//! create() always returns \c eventdestroynotify.

	template<typename ptrrefType> class objfactory : public newobj {
	};
};

#if 0
{
#endif
}
#endif
