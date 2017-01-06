/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_functionalrefptr_H
#define x_functionalrefptr_H

#include <x/functional.H>
#include <x/functionalrefptrfwd.H>
#include <x/ref.H>
#include <x/ptr.H>
#include <x/obj.H>
namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Multiply inherit from \ref function "function" and \ref obj "obj".

//! This implements a reference-counted \ref functionref "function object".

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

	//! Convenient alias.
	template<typename ...Args>
	auto invoke(Args && ...args) const
	{
		const function_t &p=*this;

		return p(std::forward<Args>(args)...);
	}
};

template<typename signature> class functionBase {

public:

	template<typename ref_type> class objfactory {

	public:

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

#ifndef DOXYGEN
extern template class functionObj<void()>;
#endif

#if 0
{
#endif
}
#endif
