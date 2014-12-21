/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_callbackobj_H
#define x_w_callbackobj_H

#include <x/obj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for a lambda wrapped into a reference-counted object.

//! A subclass captures the functor, and forwards invoke()'s
//! arguments to the functor.
//!
//! \see callback
template<typename ret_type, typename ...Args>
class callbackObj : virtual public obj {

public:
	//! Constructor
	callbackObj() {}

	//! Destructor
	~callbackObj() {}

	//! Invoke the callback functor

	virtual ret_type invoke(Args ...args)=0;
};

//! Base class for a lambda wrapped into a reference-counted object.

//! Specialization for callbacks that return a void with a default no-op
//! implementation.
//!
//! A subclass captures the functor, and forwards invoke()'s
//! arguments to the functor.
//!
//! \see callback
template<typename ...Args>
class callbackObj<void, Args...> : virtual public obj {

public:
	//! Constructor
	callbackObj() {}

	//! Destructor
	~callbackObj() {}

	//! Invoke the callback functor

	//! The default implementation does nothing.
	virtual void invoke(Args ...args) {}
};

#ifndef DOXYGEN
extern template class callbackObj<void>;
#endif

#if 0
{
#endif
}

#endif
