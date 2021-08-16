/*
** Copyright 2014-2021 Double Precision, Inc.
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
//! The template parameter is a function signature:
//!
//! \code
//! void method(const INSERT_LIBX_NAMESPACE::function< size_t(const char *)> &func);
//!
//! method(INSERT_LIBX_NAMESPACE::make_function<size_t(const char *)>(
//!      []
//!      (const char *s)
//!      {
//!          return strlen(s);
//!      }));
//! \endcode
//!
//! Alternatively,
//! \ref functionref "INSERT_LIBX_NAMESPACE::functionref" implements
//! \c INSERT_LIBX_NAMESPACE::function as a reference-counted object.


template<class> class function;

template<class> class functionObj;

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

template<typename base_class,
	 typename functor_type, typename return_type, typename ...Args>
class function_impl : public base_class {

public:

	//! The functor
	mutable functor_type functor;

	//! Constructor
	template<typename functor_arg>
	function_impl(functor_arg &&functorArg)
		: functor{std::forward<functor_arg>(functorArg)}
	{
	}

	//! Implement the function call.

	return_type operator()(Args ...args) const override
	{
		return functor(std::forward<Args>(args)...);
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

	//! The decayed functor type.

	typedef typename std::decay<functor>::type decayed_type;

	//! Signature

	typedef function_impl<function<return_type(Args...)>,
			      decayed_type,
			      return_type, Args...> type_t;

	//! Signature

	typedef function_impl<functionObj<return_type(Args...)>,
			      decayed_type,
			      return_type, Args...> objtype_t;
};

//! Construct a function object.

//! make_function<signature>(lambda) returns a subclass of
//! \ref function "INSERT_LIBX_NAMESPACE::function<signature>".

template<typename signature, typename functor>
auto make_function(functor &&functorArg)
{
	typedef typename function_impl_ret<functor, signature>::type_t type_t;

	return type_t(std::forward<functor>(functorArg));
}

#if 0
{
#endif
}
#endif
