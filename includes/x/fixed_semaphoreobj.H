/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fixed_semaphoreobj_H
#define x_fixed_semaphoreobj_H

#include <x/obj.H>
#include <x/ptrfwd.H>
#include <x/logger.H>
#include <x/mpobj.H>

namespace LIBCXX_NAMESPACE {
#if 0
}
#endif

//! A fixed semaphore

//! \see fixed_semaphore

class fixed_semaphoreObj : virtual public obj {

	//! Logger

	LOG_CLASS_SCOPE;

	//! Callback base class

	//! Acquisition and release lambdas get instantiated in a subclass
	//! that implements acquired() and release().

	class callbackBaseObj : virtual public obj {

	public:
		//! Constructor
		callbackBaseObj();

	
	    	//! Destructor
		~callbackBaseObj();

		//! Semaphore can be acquired

		virtual bool acquired()=0;

		//! Semaphore can be released
		virtual void release() noexcept=0;
	};

	//! Capture semaphore acquisition and release functors.

	//! The functors get captured in this object, the functors get
	//! invoked by acquire(), and from the acquired handle's destructor.

	template<typename acquired_functor_type,
		 typename release_functor_type>
	class callbackObj : public callbackBaseObj {
	public:
		//! The semaphore can be acquired
		acquired_functor_type acquired_functor;

		//! The semaphore can be released
		release_functor_type release_functor;

		//! Capture the functors
		template<typename Acquired, typename Released>
		callbackObj(Acquired && acquired,
			    Released && released)
			: acquired_functor(std::forward<Acquired>(acquired)),
			  release_functor(std::forward<Released>(released))
		{
		}

		//! Destructor
		~callbackObj()
		{
		}

		//! Invoke the acquired functor
		bool acquired() override
		{
			return acquired_functor();
		}

		//! Invoke the release functor
		void release() noexcept override
		{
			release_functor();
		}
	};

public:
	//! Constructor
	fixed_semaphoreObj(//! How many instances of the semaphore exist
			   size_t howmany=1);

	//! Destructor
	~fixed_semaphoreObj();

	class acquiredObj;

	//! Implement acquire()

	//! acquire() captures the functors in a callback, and invokes
	//! do_acquire().

	ptr<acquiredObj> do_acquire(size_t howmany,
				    const ref<callbackBaseObj> &callback);

	//! Attempt to acquire a semaphore, or semaphores.

	//! Returns a \ref fixed_semaphoreBase::acquiredptr "handle" for
	//! the acquired semaphore or semaphores.
	//!
	//! Returns a null handle if the semaphore cannot be immediately
	//! acquired, or if the acquired callback returns false.

	template<typename Acquired, typename Released>
	ptr<acquiredObj> acquire(//! A functor that gets invoked to acquire the semaphore or semaphores.
				 Acquired && acquired,

				 //! A functor that gets invoked to release the semaphore or semaphores.

				 //! This functor will not get called if the
				 //! semaphore cannot be acquired immediately.
				 Released && released,

				 //! How many semaphore instances can be
				 //! acquired.
				 size_t howmany=1)
	{
		return do_acquire(howmany,
				  ref<callbackObj<
				  typename std::decay<Acquired>::type,
				  typename std::decay<Released>::type>>
				  ::create(std::forward<Acquired>(acquired),
					   std::forward<Released>(released)));
	}

private:

	//! How many semaphore instances are currently available.
	mpobj<size_t> available;
};

#if 0
{
#endif
}

#endif
