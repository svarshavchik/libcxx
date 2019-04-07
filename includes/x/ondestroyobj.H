/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ondestroyobj_H
#define x_ondestroyobj_H

#include <x/obj.H>
#include <x/ptr.H>
#include <x/weakinfo.H>
#include <x/namespace.h>

#include <mutex>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Destructor reference implementation

//! This object keeps a reference to a destructor callback, and some object,
//! and adds the destructor callback to the object's destructor callback list.
//!
//! The cancel() method removes the destructor callback from the object's
//! callback list.
//!
//! An optional flag automatically invokes cancel() when this object goes
//! out of scope.

class ondestroyObj : virtual public obj {

	//! The destructor callback

	ptr<obj::destroyCallbackObj> cb;

	//! The object to which the destructor callback is attached
	ptr<weakinfo> wi;

	//! The callback's iterator in the object's callback list.

	std::list< ref<obj::destroyCallbackObj> >::iterator wi_cb_iter;

	//! Automatically invoke cancel() when this object goes out of scope.

	bool autodestroy;

	//! Mutex to protect concurrent access to this object.
	std::mutex mutex;

public:
	//! Construct a destructor reference

	ondestroyObj(//! The destructor callback object
		     const ref<obj::destroyCallbackObj> &cbArg,

		     //! The object to which the destructor callback should be added
		     const ref<obj> &objArg,

		     //! Automatically cancel the destructor when this object goes out of scope
		     bool autodestroyArg=false);

	//! Destroy this reference object

	//! The destructor callback is optionally cancelled. If not, the
	//! destructor callback remains in force for the original object.
	~ondestroyObj();

	//! Cancel the destructor callback

	//! The destructor callback will no longer be invoked when the original
	//! object goes out of scope.
	//!
	//! If the destructor callback was already invoked or cancelled,
	//! cancel() does nothing. If the object is already being destroyed
	//! by another thread, which is invoking all of its destructor callback
	//! cancel() waits until all destructor callbacks have completed.
	//! In all cases, the destructor callback is guaranteed not to be
	//! called after cancel() returns.

	void cancel();
};

#if 0
{
#endif
}
#endif
