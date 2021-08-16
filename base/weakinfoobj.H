/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakinfoobj_H
#define x_weakinfoobj_H

#include "x/namespace.h"
#include "x/ptrfwd.H"
#include "x/weakptrfwd.H"
#include "x/weakinfofwd.H"
#include "x/weakinfobaseobj.H"
#include "x/mpobj.H"
#include "x/obj.H"
#include <list>

#include <mutex>
#include <condition_variable>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

struct LIBCXX_HIDDEN weakinfo_private;

struct weakinfo_private {

	//! The underlying object.

	//! \internal The source for all the trouble. A strong reference
	//! gets constructed by taking a reference to this object. When the
	//! object gets destroyed, objp gets set to \c NULL.
	obj *objp;

	//! The destruction of objp has been aborted

	//! Beacuse another thread managed to get a strong reference.
	//! The other thread does not finish recovering the strong reference
	//! until this is set to true and the condition variable gets
	//! signaled.
	//!
	//! The thread that started destroying the object sets this flag
	//! and signals the condition variable before aborting.
	//!
	//! Since the other thread waits until the first thread finishes,
	//! this prevents the object from being destroyed again, because
	//! the other thread already incremented the reference count, and
	//! it won't proceed until the destruction finishes.
	bool destroy_aborted=false;

	//! A list of all callbacks to invoke upon destruction of object

	//! When objp gets set to \c NULL (see above) this list gets empied
	//! out.
	std::list< ondestroy_cb_t > callback_list;

	//! Constructor
	inline weakinfo_private(obj *objp) : objp{objp} {}
};

//! Weak pointer information.

//! \internal
//! Instances of weakptr point to to these objects. The class member objp points
//! to the class instance, if it exists. If not \c NULL, the strong reference
//! can be instantiated from \c objp. When the object gets destroyed, objp
//! becomes \c NULL.
//! A mutex protects the contents of these objects.

class weakinfoObj : public weakinfobaseObj {

	//! A mutex-protected object with our data.

	typedef mpcobj<weakinfo_private> weakinfo_data_t;

	weakinfo_data_t weakinfo_data;

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
	//! referring to the object being destroyed, itself, going out of
	//! scope. ondestroyObj() takes a lock on the same mutex in order to
	//! fulfill the contract of waiting for any pending destructor callbacks
	//! to return. But, if the lock is being held for the purpose of the
	//! current callback's executions, the recursive mutex prevents it
	//! from deadlocking.
	std::recursive_mutex destroy_mutex;


public:
	//! Constructor.

	weakinfoObj(//! The underlying object.

		    //! This is the object whose weak references are tracked
		    //! by this weakinfo instance.
		    obj *objp) : weakinfo_data{objp} {}

	//! Destructor.

	~weakinfoObj();

	//! Recover a strong reference to this object.

	//! \internal
	//! \return A regular, strong, reference to this object. The reference
	//! is unbound if the underlying object is already destroyed.
	ptr<obj> getstrongref() override;

	friend class obj;
	friend class ondestroyObj;
};

#if 0
{
#endif
}
#endif
