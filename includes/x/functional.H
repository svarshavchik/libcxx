/*
** Copyright 2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_functional_H
#define x_functional_H

#include <x/namespace.h>
#include <utility>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A function object implemented using virtual inheritance.

//! Instead of using std::function, which uses dynamic memory allocation,
//! use virtual inheritance to implement similar functionality.
//!
//! Typical usage.
//!
//! \code
//! void method(const INSERT_LIBX_NAMESPACE::function< int(const char *)>> &func);
//!
//! method(INSERT_LIBX_NAMESPACE::make_function<int(const char *)>(
//!      []
//!      (const char *s)
//!      {
//!          return strlen(s);
//!      }));
//! \endcode

template<class> class function;

//! Function specialization:

//! \see function

template<typename return_type, typename ...Args>
class function<return_type(Args...)> {

public:
	//! Invoke the function
	virtual return_type operator()(Args...) const=0;
};

//! Subclass of \ref function "function" that implements the virtual function call.

//! \see function

template<typename functor_type, typename return_type, typename ...Args>
class function_impl : public function<return_type(Args...)> {

public:

	//! The functor
	mutable functor_type functor;

	//! Constructor
	template<typename functor_arg>
	function_impl(const functor_arg &functorArg) : functor(functorArg)
	{
	}

	//! Move constructor
	template<typename functor_arg>
	function_impl(functor_arg &&functorArg) : functor(std::move(functorArg))
	{
	}

	//! Implement the function call.

	return_type operator()(Args ...args) const override
	{
		return functor(std::forward<Args>(args)...);
	}
};

//! Specialization for function objects that return a void.

template<typename functor_type, typename ...Args>
class function_impl<functor_type, void, Args...>
	: public function<void(Args...)> {

public:

	//! The functor
	const functor_type functor;

	//! Constructor
	template<typename functor_arg>
	function_impl(const functor_arg &functorArg) : functor(functorArg)
	{
	}

	//! Move constructor
	template<typename functor_arg>
	function_impl(functor_arg &&functorArg) : functor(std::move(functorArg))
	{
	}

	//! Implement the function call.

	void operator()(Args ...args) const override
	{
		functor(std::forward<Args>(args)...);
	}
};

//! Construct a function_impl class

//! Helper class used by make_function().
//! \internal

template<typename functor, typename signature> class function_impl_ret;

//! Specialization

template<typename functor, typename return_type, typename ...Args>
class function_impl_ret<functor, return_type(Args...)> {

public:

	//! Signature

	typedef function_impl<typename std::decay<functor>::type,
			      return_type, Args...> type_t;
};

//! Construct a function object.

//! make_function<signature>(lambda) returns a subclass of
//! \ref function "INSERT_LIBX_NAMESPACE::function<signature>".

template<typename signature, typename functor>
typename function_impl_ret<functor, signature>::type_t
make_function(const functor &functorArg)
{
	typedef typename function_impl_ret<functor, signature>::type_t type_t;

	return type_t(functorArg);
}

//! Construct a function object.

//! \see make_function

template<typename signature, typename functor>
typename function_impl_ret<functor, signature>::type_t
make_function(functor &&functorArg)
{
	typedef typename function_impl_ret<functor, signature>::type_t type_t;

	return type_t(std::move(functorArg));
}

#if 0
{
#endif
}
#endif
