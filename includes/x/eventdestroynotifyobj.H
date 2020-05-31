/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventdestroynotifyobj_H
#define x_eventdestroynotifyobj_H

#include <x/eventfd.H>
#include <x/weakptrobj.H>

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class eventdestroynotifyBase;

//! Notify an event file descriptor when an object gets destroyed

//! See #eventdestroynotify for more information.
//!

class eventdestroynotifyObj : virtual public obj {

	//! Event file descriptor to signal

	weakptr<eventfdptr> notifyeventfd;

	//! Flag - the other object was destroyed

	bool destroyedflag;

public:
	friend class eventdestroynotifyBase;

	//! Constructor
	eventdestroynotifyObj() noexcept;

	//! Destructor
	~eventdestroynotifyObj();

	//! Check if the object was destroyed

	//! \c true if the object was destroyed, otherwise wait until the
	//! event file descriptor is signaled, and try again.
	bool wasdestroyed()
	{
		std::lock_guard<std::mutex> lock(objmutex);

		return destroyedflag;
	}

private:
	//! The object was destroyed

	void destroyed();

	//! Attach this as a callback to another object's destruction

	void install(//! Notify this eventfd

		     const eventfd &eventfdArg,

		     //! Upon destruction of this object

		     const ref<obj> &objArg);
};

#if 0
{
#endif
}
#endif
