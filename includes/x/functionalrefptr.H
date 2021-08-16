/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_functionalrefptr_H
#define x_functionalrefptr_H

#include <x/functional.H>
#include <x/functionalrefptrfwd.H>
#include <x/functionalrefptrobj.H>
#include <x/ref.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class of type-erased \ref function "reference-counted functors".

//! \internal

template<typename signature> class functionBase {

public:

	//! Custom factory for \ref function "reference-counted functors".

	template<typename ref_type> class objfactory {

	public:

		//! Type-erase the functor.

		template<typename functor_type>
		static inline ref_type create(functor_type &&functor)
		{
			typedef typename
				function_impl_ret<functor_type,
						  signature>::objtype_t
				objtype_t;

			return ref<objtype_t>::create(std::forward<functor_type>
						      (functor));
		}
	};
};

//! Override refptr_traits for functioref and functionptr.

template<typename signature, typename ref_or_ptr>
class func_refptr_traits {

public:

	//! My object
	typedef functionObj<signature> obj_t;

	//! My base class
	typedef functionBase<signature> base_t;

	//! Our refs
	typedef functionref<signature> ref_t;

	//! Our ptrs
	typedef functionptr<signature> ptr_t;

	//! This type
	typedef ref_or_ptr type_t;
};

//! Only implemented for \ref functionptr "functionptr"s.

template<typename base_ref,
	 typename return_type,
	 typename ...Args>
inline functionrefptr_impl<base_ref, return_type(Args...)>
::functionrefptr_impl()=default;

//! Pretty much the default constructor.

//! Only implemented for \ref functionptr "functionptr"s.
template<typename base_ref,
	 typename return_type,
	 typename ...Args>
inline functionrefptr_impl<base_ref, return_type(Args...)>
::functionrefptr_impl(std::nullptr_t)
{
}

//! Constructor

//! The constructor parameter is a callable object, we construct a new
//! reference-counted object.

template<typename base_ref,
	 typename return_type,
	 typename ...Args>
template<is_function_ref_or_ptr_callable<return_type, Args...> F>
inline functionrefptr_impl<base_ref, return_type(Args...)>
::functionrefptr_impl(F &&f)
	: base_ref{base_ref::create(std::forward<F>(f))}
{
}

//! Constructor

//! The constructor parameter is an existing reference-counted object.

template<typename base_ref,
	 typename return_type,
	 typename ...Args>
template<is_function_ref_or_ptr<return_type(Args...)> ref_or_ptr>
inline functionrefptr_impl<base_ref, return_type(Args...)>
::functionrefptr_impl(ref_or_ptr &&r)
	: base_ref{std::forward<ref_or_ptr>(r)}
{
}

//! Implement boolean operator.

//! Only implemented for \ref functionptr "functionptr"s.

template<typename base_ref,
	 typename return_type,
	 typename ...Args>
inline functionrefptr_impl<base_ref, return_type(Args...)>
::operator bool() const noexcept
{
	return base_ref::operator bool();
}

//! Implement boolean operator.

//! Only implemented for \ref functionptr "functionptr"s.
template<typename base_ref,
	 typename return_type,
	 typename ...Args>
inline bool functionrefptr_impl<base_ref, return_type(Args...)>
::operator!() const noexcept
{
	return base_ref::operator !();
}

//! Invoke the wrapped callable object.
template<typename base_ref,
	 typename return_type,
	 typename ...Args>
template<typename ...callArgs>
inline auto functionrefptr_impl<base_ref, return_type(Args...)>
::operator()(callArgs && ...args) const
	->decltype((*std::declval<return_type (*)(Args...)>())
		   (std::forward<callArgs>(args)...))
{
	return this->operator*()(std::forward<callArgs>(args)...);
}

//! Assignment operator.

//! Assigning a new callable object, we construct a new
//! reference-counted object.

template<typename base_ref,
	 typename return_type,
	 typename ...Args>
template<is_function_ref_or_ptr_callable<return_type, Args...> F>
auto &functionrefptr_impl<base_ref, return_type(Args...)>::operator=(F &&f)
{
	base_ref::operator=(base_ref::create(std::forward<F>(f)));
	return *this;
}

//! Assignment operator.

//! Assigning from an existing reference-counted object.

template<typename base_ref,
	 typename return_type,
	 typename ...Args>
template<is_function_ref_or_ptr<return_type(Args...)> ref_or_ptr>
auto &functionrefptr_impl<base_ref, return_type(Args...)>
::operator=(ref_or_ptr &&r)
{
	base_ref::operator=(std::forward<ref_or_ptr>(r));
	return *this;
}

//! Assign a null value, effectively clearing the \ref functionptr "functionptr".

//! Only implemented for \ref functionptr "functionptr"s.
template<typename base_ref,
	 typename return_type,
	 typename ...Args>
inline auto &functionrefptr_impl<base_ref, return_type(Args...)>
::operator=(std::nullptr_t v)
{
	base_ref::operator=(v);
	return *this;
}

#ifndef DOXYGEN
extern template class functionObj<void()>;
#endif

#if 0
{
#endif
}
#endif
