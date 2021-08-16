/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_dispatchablemsgobj_H
#define x_dispatchablemsgobj_H

#include <x/namespace.h>
#include <x/ref.H>
#include <x/dispatchablebaseobj.H>
#include <type_traits>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif


//! A dispatchable message

//! Implement a dispatch() that calls some other object's (overloaded)
//! dispatch() method.
//!
//! The first template parameter specifies the pointer or the reference type
//! whose dispatch() method gets invoked. dispatch() invokes
//! obj_type->dispatch(...).
//!
//! The second template parameter gives the message class, the parameter to
//! obj_type->dispatch().
//!
//! The first argument to the constructor is an obj_type that gets saved
//! and invoked by dispatch(). Any remaining arguments to the constructor get
//! forwarded to the message class's constructor, which is multiply-inherited
//! from.
//!
//! Then, dispatch() invokes obj_type->dispatch(msg_type &);

template<typename obj_type, typename msg_type>
class dispatchablemsgObj : public dispatchablebaseObj, public msg_type {

	//! The object to dispatch the parameters to.

	obj_type objp;

public:

	//! Constructor
	template<typename ...Params>
	dispatchablemsgObj(obj_type && objpArg,
			   Params && ...paramsArg)
		: msg_type(std::forward<Params>(paramsArg)...),
		  objp(std::move(objpArg))
	{
	}

	//! Constructor
	template<typename ...Params>
	dispatchablemsgObj(const obj_type &objpArg,
			   Params && ...paramsArg)
		: msg_type(std::forward<Params>(paramsArg)...), objp(objpArg)
	{
	}

	//! Destructor
	~dispatchablemsgObj()
	{
	}

	//! Dispatch the message

	void dispatch() override
	{
		objp->dispatch(static_cast<msg_type &>(*this));
	}
};

//! Helper function that constructs a dispatch message.

//! \code
//!    INSERT_LIBX_NAMESPACE::make_dispatchablemsg<T>(ptr, args...);
//! \endcode
//!
//! This returns an object whose dispatch() method invokes
//! ptr->dispatch(const T&).
//!
//! The remaining args... get passed to T's constructor.

template<typename msg_type, typename obj_type, typename ...Args>
inline ref<dispatchablemsgObj<obj_type, msg_type> >
make_dispatchablemsg(obj_type && obj, Args && ...args)
{
	return ref< dispatchablemsgObj<obj_type, msg_type> >
		::create( std::forward<obj_type>(obj),
			  std::forward<Args>(args)...);
}

#if 0
{
#endif
}
#endif
