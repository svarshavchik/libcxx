/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_callbackfwd_H
#define x_w_callbackfwd_H

#include <x/ptrfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename ret_type, typename ...Args> class callbackObj;
template<typename ret_type, typename ...Args> class callbackBase;

template<typename ret_type, typename ...Args> class notifyBase;
template<typename ret_type, typename ...Args> class notifyObj;

//! Wrap a lambda into a reference-counted object.

//! A simple callback design pattern.
//!
//! A reference to a reference-counted object that saves a lambda into
//! a reference-counted object.
//!
//! \code
//!    auto cb=INSERT_LIBX_NAMESPACE::callback<void, int, const std::string &>::create([]
//!     (int n, const std::string &s)
//!     {
//!           // ...
//!     });
//!
//! // ...
//!
//! cb->invoke(0, "hello world");
//! \endcode
//!
//! This is the mechanism that implements various library callbacks, as
//! reference-counted objects. The first
//! template parameter is the callback's return value, the remaining parameters
//! are the callback's argument types.
//!
//! The constructor takes a lambda, whose arguments' types are the template
//! parameters. create() returns a ref handle to an object whose invoke()
//! method forwards its parameters to the lambda.

template<typename ret_type, typename ...Args>
using callback=ref<callbackObj<ret_type, Args...>,
		   callbackBase<ret_type, Args...>>;

//! A nullable pointer reference to an \ref callback "functor callback"

template<typename ret_type, typename ...Args>
using callbackptr=ptr<callbackObj<ret_type, Args...>,
		      callbackBase<ret_type, Args...>>;

//! A constant \ref callback "functor callback"
template<typename ret_type, typename ...Args>
using const_callback=const_ref<callbackObj<ret_type, Args...>,
			       callbackBase<ret_type, Args...>>;

//! A nullable pointer reference to a constant \ref callback "functor callback"

template<typename ret_type, typename ...Args>
using const_callbackptr=const_ptr<callbackObj<ret_type, Args...>,
				  callbackBase<ret_type, Args...>>;

#if 0
{
#endif
}

#endif
