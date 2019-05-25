/*
** Copyright 2012-2019 Double Precision, Inc.
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
//! typedef singleton<mySingletonObj> singleton_t;
//!
//! // ...
//!
//! ref<mySingletonObj> s=singleton_t::get();
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
	 typename baseClass> class singleton {

	//! The instance on the main thread's local object pool, and its pointer

	struct static_instance_t {

		//! Pointer to the instance.

		obj_type *instance=nullptr;

		//! The mutex.

		std::mutex instance_mutex;
	};


private:
	//! The instance on the main thread's local object pool.

	static static_instance_t static_instance;

	//! A wrapper that sets/clears the instance pointer

	//! This get stashed away into the main thread's local scope.	//!

	class wrapperObj : virtual public obj {

	public:
		//! The singleton instance
		ref<obj_type, baseClass> p;

		//! Constructor
		wrapperObj() : p{ref<obj_type, baseClass>::create()}
		{
			static_instance.instance=&*p; // Already protected
		}

		//! Destructor
		~wrapperObj()
		{
			std::unique_lock<std::mutex> lock{
				static_instance.instance_mutex
					};
			static_instance.instance=0;
		}
	};

public:

	//! Deleted copy constructor
	singleton(const singleton &)=delete;

	//! Deleted assignment operator

	singleton &operator=(const singleton &)=delete;

	//! Default no-op constructor
	singleton()=default;

	//! Return the singleton instance

	static ref<obj_type, baseClass> get()
	{
		ptr<wrapperObj> wrapped;

		std::unique_lock<std::mutex> lock{
			static_instance.instance_mutex
				};

		if (!static_instance.instance)
		{
			wrapped=ptr<wrapperObj>::create();

			run_async::localscope
				::mainscope_pushstrongobjects(wrapped);
		}

		return ref<obj_type, baseClass>{static_instance.instance};
	}
};


template<typename obj_type, typename baseClass> typename
singleton<obj_type, baseClass>::static_instance_t singleton<obj_type,
							    baseClass
							    >::static_instance;


#if 0
{
#endif
}
#endif
