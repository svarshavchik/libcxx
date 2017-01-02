/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_singleton_H
#define x_singleton_H

#include <x/namespace.h>
#include <x/threads/run.H>
#include <x/singletonfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Singleton class template

//! \code
//! class mySingletonObj : virtual public obj {
//!
//! public:
//!    mySingletonObj();
//!    ~mySingletonObj();
//! };
//!
//! singleton<mySingletonObj> singleton_obj;
//!
//! // ...
//!
//! ref<mySingletonObj> s=singleton_obj.get();
//! \endcode
//!
//! This template instantiates exactly one instance of a class. The class must
//! be virtually derived from obj. get() returns the reference to a single
//! instance of the class.
//!
//! The first call to get() instantiates the class. Subsequent calls to get()
//! return a reference to the same class. The single class instance gets
//! destroyed when the application terminates, provided that all active
//! references to the singleton instance go out of scope.

template<typename obj_type,
	 typename baseClass> class singleton : public notcopyableObj {

	//! The instance on the main thread's local object pool.

	static obj_type *instance;

	//! A mutex that protects instance

	static std::mutex instance_mutex;

	//! A wrapper that sets/clears the instance pointer

	//! This get stashed away into the main thread's local scope.
	//!

	class wrapperObj : virtual public obj {

	public:
		//! The singleton instance
		ref<obj_type, baseClass> p;

		//! Constructor
		wrapperObj() : p(ref<obj_type, baseClass>::create())
		{
			instance=&*p; // Already protected
		}

		//! Destructor
		~wrapperObj()
		{
			std::unique_lock<std::mutex> lock(instance_mutex);
			instance=0;
		}
	};

public:

	//! Default no-op constructor
	singleton() {}

	//! Return the singleton instance

	//! This may return a null ptr during application startup or shutdown.
	//!

	static ref<obj_type, baseClass> get()
	{
		ptr<wrapperObj> wrapped;

		std::unique_lock<std::mutex> lock(instance_mutex);

		if (!instance)
		{
			wrapped=ptr<wrapperObj>::create();

			run_async::localscope
				::mainscope_pushstrongobjects(wrapped);
		}

		return (ref<obj_type, baseClass>(instance));
	}
};

template<typename obj_type,
	 typename baseClass>
obj_type *singleton<obj_type, baseClass>::instance=0;

template<typename obj_type,
	 typename baseClass>
std::mutex singleton<obj_type, baseClass>::instance_mutex;

#if 0
{
#endif
}
#endif
