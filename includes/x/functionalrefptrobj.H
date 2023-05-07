/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_functionalrefptrobj_H
#define x_functionalrefptrobj_H

#include <x/functionalrefptrfwd.H>
#include <x/refptr_traits.H>
#include <x/functional.H>
#include <type_traits>
#include <functional>
#include <x/obj.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Multiply inherit from \ref function "function" and \ref obj "obj".

//! This implements a reference-counted \ref functionref "function object".

//! \internal

template<typename signature> class functionObj
	: public function<signature>, virtual public obj {

public:

	//! Default constructor
	functionObj()=default;

	//! Default destructor
	~functionObj()=default;

	//! The implementing function wrapper.
	typedef function<signature> function_t;

	using function_t::operator();
};

//! \anchor functionalrefptr_impl_spec Implementation of a \ref functionref "reference-counted function object".

//! Inherits of a \ref ref "ref" or a \ref ptr "ptr" and implements the \c () operator.

template<typename base_ref,
	 typename return_type,
	 typename ...Args>
class functionrefptr_impl<base_ref, return_type(Args...)>
	: public base_ref {

	using base_ref::create;

	//! Alias
	typedef functionrefptr_impl<base_ref, return_type(Args...)> class_t;

public:

	//! My traits
	typedef func_refptr_traits<return_type(Args...), class_t> traits_t;

	//! Default constructor.

	//! Only implemented for \ref functionptr "functionptr"s.

	functionrefptr_impl();

	//! Pretty much the default constructor.

	//! Only implemented for \ref functionptr "functionptr"s.
	functionrefptr_impl(std::nullptr_t);

	//! Constructor with a callable object.
	template<is_function_ref_or_ptr_callable<return_type, Args...> F>
	functionrefptr_impl(F &&f);

	//! Constructor with another ref/ptr
	template<is_function_ref_or_ptr<return_type(Args...)> ref_or_ptr>
	functionrefptr_impl(ref_or_ptr &&r);

	using base_ref::operator *;
	using base_ref::operator ==;
	using base_ref::operator <=>;

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

	template<is_function_ref_or_ptr_callable<return_type, Args...> F>
	auto &operator=(F &&f);

	template<is_function_ref_or_ptr<return_type(Args...)> ref_or_ptr>
	auto &operator=(ref_or_ptr &&r);

	//! Assign a null value, effectively clearing the \ref functionptr "functionptr".

	//! Only implemented for \ref functionptr "functionptr"s.
	auto &operator=(std::nullptr_t v);
};

#if 0
{
#endif
}

#define x_functionalrefptrobj_H_defined 1
#include <x/functionalrefptr_hash.H>

#endif
