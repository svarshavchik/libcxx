/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_functionalrefptr_H
#define x_functionalrefptr_H

#include <x/functional.H>
#include <x/ref.H>
#include <x/ptr.H>
#include <x/obj.H>
namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Multiply inherit from \ref function "function" and \ref obj "obj".

//! This implements a reference-counted \ref function "function" object.
//! Use \ref make_functionref "make_functionref"() to construct this object.

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

//! A reference-counted \ref function "function object".

//! Use \ref make_functionref "make_functionref()" to construct this
//! reference.

template<typename signature>
using functionref=ref<functionObj<signature>>;

//! A nullable pointer to a \ref functionref "reference to a function object".
template<typename signature>
using functionptr=ptr<functionObj<signature>>;

//! Construct a function object.

//! make_functionref<signature>(lambda) returns a subclass of a
//! \ref functionObj "INSERT_LIBX_NAMESPACE::functionObj<signature>".
//!
//! Typical usage:
//!
//! \code
//!
//! INSERT_LIBX_NAMESPACE::functionref<int(const char *)> ref=INSERT_LIBX_NAMESPACE::make_functionref(
//!                                           (const char *p)
//!                                           {
//!                                                 return strlen(p);
//!                                           });
//! INSERT_LIBX_NAMESPACE::function< int(const char *)>> &p= *ref;
//!
//! int l=p("foo");
//!
//! int l=ref->invoke("bar");
//! \endcode
//!
//! make_functionref() constructs a
//1 \ref ref "reference-counted object" that inherits from
//! \ref function "INSERT_LIBX_NAMESPACE::function". It serves the
//! same type-erasure function as
//! \ref function "INSERT_LIBX_NAMESPACE::function", but as a reference-counted
//! object.
//!
//! This is a reference to a \ref functionObj "functionObj<signature>".
//! In addition to having the \c operator() operator available,
//! the \c invoke() method offers a more convenient syntax of invoking the
//! type-erased lambda, using the reference pointer.

template<typename signature, typename functor>
auto make_functionref(functor &&functorArg)
{
	typedef typename
		function_impl_ret<functor, signature>::objtype_t objtype_t;

	return functionref<signature>
		( ref<objtype_t>::create(std::forward<functor>
					 (functorArg)));
}

#if 0
{
#endif
}
#endif
