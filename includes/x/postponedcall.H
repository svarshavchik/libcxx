/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

/*!

This file defines the make_postponedcall() template function.

*/

#ifndef x_postponedcall_H
#define x_postponedcall_H

#include <x/namespace.h>
#include <x/tuple_param_pack.H>
#include <x/obj.H>
#include <x/ref.H>
#include <tuple>
#include <type_traits>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Helper class for unpacking a postponed call's parameter pack

template<typename ParamPackArgsType> class postponedCallbackHelper;

//! Specialization of a helper class for unpacking a postponed call's parameter pack

template<size_t ...n>
class postponedCallbackHelper<tuple_param_pack<n...> > {

public:

	//! Unpack the postponed call's tuple parameter pack, then invoke it.

	template<typename callbackType, typename tupleType>
	static inline auto invoke(//! The postponed callback object
				  callbackType &callback,

				  //! The postponed call's parameters
				  tupleType &tuple)
		-> decltype(callback(std::get<n>(tuple)...))
	{
		return callback(std::get<n>(tuple)...);
	}
};

//! Base class for a postponed call

//! The template parameter is the postponed call's return value. Defines
//! invoke(), implemented in the subclass, that makes the postponed call.

template<typename callbackRetType>
class postponedcallBaseObj : virtual public obj {

public:
	postponedcallBaseObj() {}
	~postponedcallBaseObj() {}

	//! Invoke the postponed call.

	virtual callbackRetType invoke()=0;
};

//! A postponed call

//! A reference to this object gets created by make_psotponedcall().
//! Its stores an opaque callback object, and the postponed call's parameters.
//! invoke() invokes the callback objects operator()(arguments), essentially.

template<typename callbackType, typename retType, typename ...Args>
class postponedcallObj : public postponedcallBaseObj<retType> {

	//! The postponed callback object.
	callbackType callback;

	//! The postponed call's arguments

	std::tuple<Args...> callback_args;

	//! Helper class for unpacking the tuple-d parameters.

	typedef postponedCallbackHelper<typename tuple_2_param_pack<
						sizeof...(Args)
						>::type> helper_t;

public:

	//! Constructor

	//! Saves the callback objects, and the parameters, for later.

	template<typename ...ForwardedArgs>
	postponedcallObj(callbackType && callbackArg,
			 ForwardedArgs && ...args)
		: callback(callbackArg),
		  callback_args(std::forward<ForwardedArgs>(args)...)
	{
	}

	//! Constructor

	//! Saves the callback objects, and the parameters, for later.
	template<typename ...ForwardedArgs>
	postponedcallObj(const callbackType &callbackArg,
			 ForwardedArgs && ...args)
		: callback(callbackArg),
		  callback_args(std::forward<ForwardedArgs>(args)...)
	{
	}

	~postponedcallObj()
	{
	}

	//! Invoke the postponed call.
	retType invoke() override
	{
		return helper_t::invoke(callback, callback_args);
	}
};

//! A postponed function call.

//! Take an opaque call, a function call, or an object with an
//! operator()(args...), and the argumenst to the function call. The call is
//! not made, rather its "frozen".
//!
//! make_postponedcall() returns a reference to a reference-counted object,
//! that can be freely passed around, with a minimum of overhead, and
//! calling ref->invoke() makes the original function call, with the original
//! arguments.
//!
//! The reference returned by make_postponedcall() is quite complicated.
//! If the callback's return value type is known, the return value from
//! make_postponedcall() can be converted to
//! \c INSERT_LIBX_NAMESPACE::ref<postponedcallBaseObj<T>>, where "T" is the
//! return type.

template<typename callbackType, typename ...Args>
auto make_postponedcall(//! A callback object

			//! This can be an ordinary function pointer, or
			//! a class with a suitable operator()(...). This
			//! includes lambdas.
			//!
			//! The callback object must be copy-constructible.
			callbackType && callback,

			//! Arguments to the function call

			//! These arbitrary arguments must also be
			//! copy-constructible, if they are objects.
			Args && ...args)
	-> ref< postponedcallObj
		<typename std::decay<callbackType>::type,
		 decltype(callback(std::forward<Args>(args)...)),
		 typename std::decay<Args>::type...> >
{
	return ref< postponedcallObj
		    <typename std::decay<callbackType>::type,
		     decltype(callback(std::forward<Args>(args)...)),
		     typename std::decay<Args>::type...>
		    >::create(callback, std::forward<Args>(args)...);
}

//! A postponed function call.

//! \overload

template<typename callbackType, typename ...Args>
auto make_postponedcall(const callbackType &callback,
			Args && ...args)
	-> ref< postponedcallObj
		<typename std::decay<callbackType>::type,
		 decltype(((callbackType)callback)(std::forward<Args>(args)...)),
		 typename std::decay<Args>::type...> >
{
	return ref< postponedcallObj
		    <typename std::decay<callbackType>::type,
		     decltype(((callbackType)callback)(std::forward<Args>(args)...)),
		     typename std::decay<Args>::type...>
		    >::create(callback, std::forward<Args>(args)...);
}

#if 0
{
#endif
}
#endif
