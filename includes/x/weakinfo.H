/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakinfo_H
#define x_weakinfo_H

#include <x/namespace.h>
#include <x/ptrfwd.H>
#include <x/weakptrfwd.H>
#include <x/obj.H>
#include <list>

#include <mutex>
#include <condition_variable>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<class refType> class weakptr;

//! Weak pointer information.

//! \internal
//! Instances of weakptr point to to these objects. The class member objp points
//! to the class instance, if it exists. If not \c NULL, the strong reference
//! can be instantiated from \c objp. When the object gets destroyed, objp
//! becomes \c NULL.
//! A mutex protects the contents of these objects.

class weakinfo : public obj {

	//! A %mutex that protects this weakinfo object.

	//! \internal Access to this object's contents is protected by locking
	//! this %mutex.

	mutable std::mutex mutex;

	//! A %mutex that's held when ondestroy callback hooks get invoked

	//! \internal
	//! This %mutex is held when the last reference to an object is in
	//! the process of going away, the object is being destroyed, and its
	//! destructor hooks are going to be invoked.
	//!
	//! This %mutex gets held before the general mutex.
	//! The general mutex gets released after callback_list is copied
	//! elsewhere, and is emptied out, then the general mutex gets released,
	//! but the destroy_mutex remains held while all the hooks are
	//! being invoked.
	//!
	//! The recursive mutex is for the benefit of
	//! \ref ondestroyObj::cancel() "INSERT_LIBX_NAMESPACE::ondestroyObj::cancel"().
	//! This enables a destructor callback to result in
	//! an \ref ondestroy "INSERT_LIBX_NAMESPACE::ondestroy" instance,
	//! referring to the object being destroy, itself going out of
	//! scope. ondestroyObj() takes a lock on the same mutex in order to
	//! fulfill the contract of waiting for any pending destructor callbacks
	//! to return. But, if the lock is being held for the purpose of the
	//! current callback's executions, the recursive mutex prevents it
	//! from deadlocking.
	std::recursive_mutex destroy_mutex;

	//! A condition variable that resolves certain race conditions.

	//! \internal This condition variable gets used to handle a race
	//! condition that occurs when the last strong reference to an object
	//! gets destroyed at the same time as another thread is attempting
	//! to obtain a strong reference, from a weak one.

	mutable std::condition_variable cond;

	//! The underlying object.

	//! \internal The source for all the trouble. A strong reference
	//! gets constructed by taking a reference to this object. When the
	//! object gets destroyed, objp gets set to \c NULL.
	mutable obj *volatile objp;

	//! Recover a strong reference to this object.

	//! \internal
	//! \return A regular, strong, reference to this object. The reference
	//! is unbound if the underlying object is already destroyed.
	ptr<obj> getstrongref() const;

	//! A list of all callbacks to invoke upon destruction of object

	std::list< ref<obj::destroyCallbackObj> > callback_list;

public:
	//! Constructor.

	weakinfo(//! The underlying object.

		 //! This is the object whose weak references are tracked
		 //! by this weakinfo instance.
		 obj *objpArg) : objp(objpArg) {}

	//! Destructor.

	~weakinfo();

	friend class obj;
	friend class ondestroyObj;
	template<class weakType> friend class weakptr;
};

#if 0
{
#endif
}
#endif
