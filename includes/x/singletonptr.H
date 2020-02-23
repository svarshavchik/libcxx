/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_singletonptr_H
#define x_singletonptr_H

#include <x/namespace.h>
#include <x/weakptr.H>
#include <x/mpobj.H>

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

	static weakptr<ptr<ref_type, baseClass>> global;

public:

	//! Explicit constructor
	singletonptr(const ptr<ref_type, baseClass> &p)
		: ptr<ref_type, baseClass>{p}
	{
		global=p;
	}

	//! Default constructor
	singletonptr() : ptr<ref_type, baseClass>{global.getptr()}
	{
	}

	using ptr<ref_type, baseClass>::operator*;

	using ptr<ref_type, baseClass>::operator->;

	using ptr<ref_type, baseClass>::operator bool;

	using ptr<ref_type, baseClass>::operator !;
};

// Instantiate the static class member.

template<typename ref_type, typename baseClass>
weakptr<ptr<ref_type, baseClass>> singletonptr<ref_type, baseClass>::global;

#if 0
{
#endif
}
#endif
