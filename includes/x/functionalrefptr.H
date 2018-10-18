/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_functionalrefptr_H
#define x_functionalrefptr_H

#include <x/functional.H>
#include <x/functionalrefptrfwd.H>
#include <x/ref.H>
#include <x/ptr.H>
#include <x/refptr_traits.H>
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

//! Determine the argument to \ref functionrefptr_impl "functionrefptr_impl".

//! The constructor or the assignment operator's parameter is either another
//! \c functionrefptr_impl "functionrefptr_impl"; or a callable object,
//! otherwise.
//!
//! Assume some other callable object, by default.
//!
//! \internal

template<typename r>
class is_functionrefptr_t : public std::false_type {};

//! Determine the argument to \ref functionrefptr_impl "functionrefptr_impl".

//! Specialization that identifies the assignment operator's parameter as
//! another \c functionrefptr_impl "functionrefptr_impl".
//!
//! \internal

template<typename r, typename signature>
class is_functionrefptr_t<functionrefptr_impl<r, signature>>
	: public std::true_type {};

//! Decay the template parameter to \ref is_functionrefptr_t "is_functionrefptr_t".

template<typename r>
using is_functionrefptr=is_functionrefptr_t<std::decay_t<r>>;

template<typename base_ref,
	 typename ref_or_ptr,
	 typename=void> struct functionrefptr_conv;

//! The argument to \ref functionrefptr_impl "functionrefptr_impl" is another instance.

//! The argument passes through unchanged, resulting in an assignment operator
//! or copy construction.

template<typename base_ref,
	 typename ref_or_ptr>
struct functionrefptr_conv<base_ref, ref_or_ptr,
			   std::enable_if_t<is_functionrefptr<ref_or_ptr>
					    ::value>> {

	//! Pass through the parameter.

	template<typename f>
	inline static auto &&conv(f &&v)
	{
		return v;
	}
};

//! The argument to \ref functionrefptr_impl "functionrefptr_impl" is a callable object.

template<typename base_ref,
	 typename ref_or_ptr>
struct functionrefptr_conv<base_ref, ref_or_ptr,
			   std::enable_if_t<std::negation_v
					    <is_functionrefptr<ref_or_ptr>>>> {

	//! Construct a reference-counted object to wrap the parameter.

	template<typename f>
	inline static auto conv(f &&v)
	{
		return base_ref::create(std::forward<f>(v));
	}
};

//! SFINAE for \ref functionref "reference-counted function object"'s constructor overload resolution.

//! \internal
template<typename functionrefptr_impl_t,
	 typename constructor_parameter_t,
	 typename ...Args>
struct functionrefptr_impl_constructible<
	functionrefptr_impl_t,
	constructor_parameter_t,
	std::tuple<Args...>,
	std::void_t<decltype(std::declval<constructor_parameter_t &&>()
			     (std::declval<Args &&>()...))>> {

	typedef void type_t;
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

//! Determines whether this instance should be copy-constructed
//! from another, existing, function object; or create a new one
//! that uses the callable object that gets passed as a parameter.

template<typename base_ref,
	 typename return_type,
	 typename ...Args>
template<typename ref_or_ptr,
	 typename>
inline functionrefptr_impl<base_ref, return_type(Args...)>
::functionrefptr_impl(ref_or_ptr &&v)
	: base_ref{functionrefptr_conv<base_ref, ref_or_ptr>
		::conv(std::forward<ref_or_ptr>(v))}
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

//! Figures out if this is assigning from a different reference-counted
//! function object, or from another callable object.

template<typename base_ref,
	 typename return_type,
	 typename ...Args>
template<typename ref_or_ptr,
	 typename>
inline auto &functionrefptr_impl<base_ref, return_type(Args...)>
::operator=(ref_or_ptr &&arg)
{
	base_ref::operator=(functionrefptr_conv<base_ref, ref_or_ptr>
			    ::conv(std::forward<ref_or_ptr>(arg)));
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


//! Implement ref and ptr traits for \ref functionref "function objects".

template<typename refptr_type, typename signature>
class refptr_traits<functionrefptr_impl<refptr_type, signature>>
	: private refptr_traits<refptr_type> {

	typedef refptr_traits<refptr_type> superclass_t;

public:
	using superclass_t::obj_t;

	using superclass_t::base_t;

	typedef functionrefptr_impl<typename superclass_t::ref_t,
				    signature> ref_t;

	typedef functionrefptr_impl<typename superclass_t::ptr_t,
				    signature> ptr_t;
};

#ifndef DOXYGEN
extern template class functionObj<void()>;
#endif

#if 0
{
#endif
}
#endif
