/*
** Copyright 2020-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_mpthreadlockobj_h
#define x_mpthreadlockobj_h

#include <x/mpthreadlockfwd.H>
#include <x/mpobj.H>
#include <x/obj.H>

namespace LIBCXX_NAMESPACE {
#if 0
}
#endif

//! Implementation object.

//! \see mpthreadlock
template<typename obj_type, typename mutex_type>
class mpthreadlockObj: virtual public obj {

	//! Alias

	typedef mpobjimpl<obj_type, mutex_type, mpobj_type::threadlock> mpobj_t;

	//! My meta
	mpobj_t &objp;

	//! Opaque mcguffin.
	const ref<obj> mcguffin;

	// This is accessed only when holding a lock on the counter:

	bool locked{false};

public:

	friend mpobj_t;

	//! Constructor

	//! Increments the thread lock counter.
	mpthreadlockObj(const mpobjimpl_lock<obj_type, mutex_type,
			mpobj_type::threadlock> &lock, const ref<obj> &mcguffin)
		: objp{lock.objp}, mcguffin{mcguffin}
	{
		mpcobj<size_t>::lock counter{objp.meta.thread_lock_counter};

		++*counter;
		locked=true;
	}

	//! Destructor
	~mpthreadlockObj()
	{
		release();
	}

	friend class mpobjimpl_lock<obj_type, mutex_type,
				    mpobj_type::threadlock>;

private:

	//! Deactivate this thread lock

	//! Invoked from the destructor or when constructing a new
	//! lock on the mutex-protected object using this thread lock.
	//!
	//! The first time this is called this will decrement the thread
	//! lock counter.
	void release()
	{
		mpcobj<size_t>::lock
			lock{objp.meta.thread_lock_counter};

		// locked is accessible now.
		if (locked)
		{
			locked=false;

			--*lock;
			lock.notify_one();
		}
	};
};

#if 0
{
#endif
}

#endif
