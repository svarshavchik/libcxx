/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_singletonptr_H
#define x_singletonptr_H

#include <x/namespace.h>
#include <x/weakptr.H>
#include <pthread.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A singleton reference.

//! \code
//! class global_dataObj : virtual public LIBCXX_NAMESPACE::obj {
//! };
//!
//! typedef singletonptr<global_dataObj> global_data_t;
//!
//! // ...
//!
//! int main()
//! {
//!      global_data_t global_data{ ref<global_dataObj>::create() };
//!
//!      // ...
//! }
//!
//! // ...
//!
//! if (foo())
//! {
//!     global_data_t global_data;
//!
//!     if (global_data)
//!     {
//!           global_data->method();
//!     }
//! }
//! \endcode
//!
//! A singletonptr wraps a global reference to an object. Instances of
//! singletonptr should always be constructed in automatic scope.
//!
//! Passing a ref to an existing object to singletonptr's constructor
//! makes the object available to any other instances of default-constructed
//! singletonptr. The referenced object remains available until all instances
//! of singletonptr go out of their automatic scope.
//!
//! A singletonptr is a subclass of a \c ptr. The default constructor results
//! in a null \c ptr if no other existing instance of a singleton exists
//! which was constructed with an explicit object.

template<typename ref_type, typename baseClass=ptrref_base>
class singletonptr : public ptr<ref_type, baseClass> {

	//! The global instance.

	//! Reverting to pthread_mutex_t, since there is no guarantee that
	//! std::mutex is trivially destructible, and the singleton can be
	//! in static scope, and global destruction phase can begin while
	//! some thread is still in its death throes.

	static pthread_mutex_t &get_instance_mutex()
	{
		static pthread_mutex_t m=PTHREAD_MUTEX_INITIALIZER;
		return m;
	}

	static ref_type *&get_object()
	{
		static ref_type *object=nullptr;

		return object;
	}

	//! Whether this is the initial one.

	bool initial;

public:

	//! Explicit constructor
	singletonptr(const ptr<ref_type, baseClass> &p)
		: ptr<ref_type, baseClass>{p}, initial{true}
	{
		if (!p)
			return;

		auto &instance_mutex=get_instance_mutex();

		pthread_mutex_lock(&instance_mutex);
		get_object()=&*p;
		pthread_mutex_unlock(&instance_mutex);
	}

	//! Default constructor
	singletonptr() : initial{false}
	{
		auto &instance_mutex=get_instance_mutex();

		pthread_mutex_lock(&instance_mutex);
		ptr<ref_type> p{get_object()};
		pthread_mutex_unlock(&instance_mutex);

		ptr<ref_type, baseClass>::operator=(p);
	}

	//! Destructor
	~singletonptr()
	{
		if (initial)
		{
			auto &instance_mutex=get_instance_mutex();

			pthread_mutex_lock(&instance_mutex);
			get_object()=nullptr;
			pthread_mutex_unlock(&instance_mutex);
		}
	}
	using ptr<ref_type, baseClass>::operator*;

	using ptr<ref_type, baseClass>::operator->;

	using ptr<ref_type, baseClass>::operator bool;

	using ptr<ref_type, baseClass>::operator !;
};

#if 0
{
#endif
}
#endif
