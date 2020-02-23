/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_functionalrefptrfwd_H
#define x_functionalrefptrfwd_H

#include <x/ptrfwd.H>
#include <x/functional.H>
#include <type_traits>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template<typename signature> class functionBase;

template<typename type,
	 typename signature> class functionrefptr_impl;

//! SFINAE for \ref functionref "reference-counted function object"'s constructor overload resolution.

//! \internal

template<typename functionrefptr_impl_t,
	 typename constructor_parameter_t,
	 typename tuple_args_t,
	 typename=void> struct functionrefptr_impl_constructible;

//! Implementation of a \ref functionref "reference-counted function object".

//! Inherits of a \ref ref "ref" or a \ref ptr "ptr" and implements the \c () operator.

template<typename base_ref,
	 typename return_type,
	 typename ...Args>
class functionrefptr_impl<base_ref, return_type(Args...)>
	: public base_ref {

	using base_ref::create;

	typedef functionrefptr_impl<base_ref, return_type(Args...)> class_t;

public:

	//! Default constructor.

	//! Only implemented for \ref functionptr "functionptr"s.

	functionrefptr_impl();

	//! Pretty much the default constructor.

	//! Only implemented for \ref functionptr "functionptr"s.
	functionrefptr_impl(std::nullptr_t);

	//! Constructor

	//! Determines whether this instance should be copy-constructed
	//! from another, existing, function object; or create a new one
	//! that uses the callable object that gets passed as a parameter.

	template<typename ref_or_ptr,
		 typename=typename functionrefptr_impl_constructible
		 <class_t, std::decay_t<ref_or_ptr>, std::tuple<Args...>>
		 ::type_t>
	functionrefptr_impl(ref_or_ptr &&v);

	using base_ref::operator *;

	//! Implement boolean operator.

	//! Only implemented for \ref functionptr "functionptr"s.

	explicit operator bool() const noexcept;

	//! Implement boolean operator.

	//! Only implemented for \ref functionptr "functionptr"s.
	bool operator!() const noexcept;

	//! Invoke the wrapped callable object.
	template<typename ...callArgs>
	auto operator()(callArgs && ...args) const
		->decltype((*std::declval<return_type (*)(Args...)>())
			   (std::forward<callArgs>(args)...));

	//! Assignment operator.

	//! Figures out if this is assigning from a different reference-counted
	//! function object, or from another callable object.

	template<typename ref_or_ptr,
		 typename=typename functionrefptr_impl_constructible
		 <class_t, std::decay_t<ref_or_ptr>, std::tuple<Args...>>
		 ::type_t>
	auto &operator=(ref_or_ptr &&arg);

	//! Assign a null value, effectively clearing the \ref functionptr "functionptr".

	//! Only implemented for \ref functionptr "functionptr"s.
	auto &operator=(std::nullptr_t v);
};

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

#if 0
{
#endif
}
#endif
