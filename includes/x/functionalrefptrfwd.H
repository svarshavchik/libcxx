/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_functionalrefptrfwd_H
#define x_functionalrefptrfwd_H

#include <x/ptrfwd.H>
#include <x/refptr_traits.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename signature> class functionBase;
template<typename signature> class functionObj;

template<typename type,
	 typename signature> class functionrefptr_impl;

//! A reference-counted \ref function "function object".

//! Implement type-erasure of callable object, like \c std::function
//! and \ref function "INSERT_LIBX_NAMESPACE::function",
//! but based on a reference-counted object derived from
//! \ref functionObj "INSERT_LIBX_NAMESPACE::functionObj<signature>".
//!
//! Typical usage of a callable object:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::functionref<size_t(const char *)> r{[]
//!                                        (const char *p)
//!                                        {
//!                                              return strlen(p);
//!                                        }};
//!
//! auto r2=r;
//!
//! INSERT_LIBX_NAMESPACE::function< size_t(const char *)> &p= *r;
//!
//! int l=p("foo");
//!
//! int l=r("bar");
//! \endcode
//!
//! This is a
//1 \ref ref "reference-counted object" that inherits from
//! \ref function "INSERT_LIBX_NAMESPACE::function". It serves the
//! same type-erasure purpose as
//! \ref function "INSERT_LIBX_NAMESPACE::function", but as a reference-counted
//! object.
//!
//! This means that, once constructed, copying
//! \c INSERT_LIBX_NAMESPACE::functionref
//! does not create a new function object. In the above example \c r2 and
//! \c r reference the same function object. A callable object may have
//! non-trivial amount of captured members, but copying it is cheap, and
//! involves only updating the internal reference count.
//!
//! This is a reference to a \ref functionObj "functionObj<signature>".
//!
//! \see functionObj
//! \see functionptr

template<typename signature>
using functionref=functionrefptr_impl<ref<functionObj<signature>,
					  functionBase<signature>>,
				      signature>;

//! A nullable pointer to a \ref functionref "reference to a function object".

//! \see functionref
template<typename signature>
using functionptr=functionrefptr_impl<ptr<functionObj<signature>,
					  functionBase<signature>>,
				      signature>;


template<typename signature, typename ref_or_ptr>
class func_refptr_traits;

//! Helper SFINAE for is_function_ref_or_ptr

//! Determines whether T is a compatible functionref.

template<typename T, typename signature, typename=void>
struct is_function_ref_or_ptr_t : std::false_type {};

//! Helper SFINAE for is_function_ref_or_ptr

//! Determines whether T is a compatible functionref.

template<typename T, typename signature>
struct is_function_ref_or_ptr_t<
	T, signature,
	std::void_t<typename refptr_traits<std::decay_t<T>>::ref_t>>
	: std::is_same<typename refptr_traits<std::decay_t<T>>::ref_t,
		       functionref<signature>> {};

//! Determine the argument to \ref functionrefptr_impl "functionrefptr_impl".

//! The constructor or the assignment operator's parameter is another
//! \c functionrefptr_impl "functionrefptr_impl".
//!
//! \internal

template<typename r, typename signature>
concept is_function_ref_or_ptr=is_function_ref_or_ptr_t<r, signature>::value;

//! Determine the argument to \ref functionrefptr_impl "functionrefptr_impl".

//! The constructor or the assignment operator's parameter is not another
//! \c functionrefptr_impl "functionrefptr_impl", but is a compatible
//! callable object, so we will create a new object.
//!
//! A functionref is also a callable object, so we first check that it's
//! not is_function-ref_or_ptr, then check for callability.
//!
//! \internal

template<typename T,
	 typename R,
	 typename ...Args>
concept is_function_ref_or_ptr_callable=
	!is_function_ref_or_ptr_t<T, R(Args...)>::value &&
	std::is_invocable_r_v<R, T, Args...>;

#if 0
{
#endif
}
#endif
