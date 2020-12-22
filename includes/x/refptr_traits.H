/*
** Copyright 2016-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_refptr_traits_H
#define x_refptr_traits_H

#include <x/namespace.h>
#include <x/ptrfwd.H>
#include <type_traits>
#include <utility>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

/*! \file

This file defines the \c refptr_traits template, that describes a
\c INSERT_LIBX_NAMESPACE::ref,
\c INSERT_LIBX_NAMESPACE::ptr,
\c INSERT_LIBX_NAMESPACE::const_ref,
\c INSERT_LIBX_NAMESPACE::const_ptr.

*/


//! Base class that refptr_traits inherits from.

template<typename objClass,
	 typename baseClass,
	 typename ref_or_ptr>
class std_refptr_traits {

public:
	//! The parent ref or ptr class of what was specified for refptr_traits
	typedef ref_or_ptr type_t;

	//! The reference-counted object type.
	typedef objClass obj_t;

	//! The base class for a \ref ref "INSERT_LIBX_NAMESPACE::ref", \ref ptr "INSERT_LIBX_NAMESPACE::ptr",  \ref const_ref "INSERT_LIBX_NAMESPACE::const_ref", or a \ref const_ptr "INSERT_LIBX_NAMESPACE::const_ptr".

	typedef baseClass base_t;

	//! The \c ref type.
	typedef ref<objClass, baseClass> ref_t;

	//! The \c ptr type.
	typedef ptr<objClass, baseClass> ptr_t;

	//! The \c const_ref type.
	typedef const_ref<objClass, baseClass> const_ref_t;

	//! The \c const_ptr type.
	typedef const_ptr<objClass, baseClass> const_ptr_t;
};

template<typename T, typename TT> class refptr_traits_impl;

//! The traits of a reference or a pointer object.

//! \see std_refptr_traits

template<typename T>
using refptr_traits=typename T::traits_t;

template<typename T> class ref_cast_lookup;
template<typename T> class ptr_cast_lookup;

//! Helper specialization used by \ref ref_cast "ref_cast".

template<typename objClass, typename baseClass>
class ref_cast_lookup<ref<objClass, baseClass>> {
public:
	typedef ref<objClass, baseClass> type;
};

//! Helper specialization used by \ref ref_cast "ref_cast".

template<typename objClass, typename baseClass>
class ref_cast_lookup<const_ref<objClass, baseClass>> {
public:
	typedef const_ref<objClass, baseClass> type;
};

//! Helper specialization used by \ref ref_cast "ref_cast".

template<typename objClass, typename baseClass>
class ref_cast_lookup<ptr<objClass, baseClass>> {
public:
	typedef ref<objClass, baseClass> type;
};

//! Helper specialization used by \ref ref_cast "ref_cast".

template<typename objClass, typename baseClass>
class ref_cast_lookup<const_ptr<objClass, baseClass>> {
public:
	typedef const_ref<objClass, baseClass> type;
};

//! Helper specialization used by \ref ptr_cast "ptr_cast".

template<typename objClass, typename baseClass>
class ptr_cast_lookup<ref<objClass, baseClass>>{
public:
	typedef ptr<objClass, baseClass> type;
};

//! Helper specialization used by \ref ptr_cast "ptr_cast".

template<typename objClass, typename baseClass>
class ptr_cast_lookup<const_ref<objClass, baseClass>> {
public:
	typedef const_ptr<objClass, baseClass> type;
};

//! Helper specialization used by \ref ptr_cast "ptr_cast".

template<typename objClass, typename baseClass>
class ptr_cast_lookup<ptr<objClass, baseClass>>{
public:
	typedef ptr<objClass, baseClass> type;
};

//! Helper specialization used by \ref ptr_cast "ptr_cast".

template<typename objClass, typename baseClass>
class ptr_cast_lookup<const_ptr<objClass, baseClass>>{
public:
	typedef const_ptr<objClass, baseClass> type;
};

//! Convert a \c ptr to an equivalent \c ref.

//! If the template parameter is a \c ref or a \c ptr, this resolves to
//! an equivalent \c ref type.
//!
//! If the template parameter is a \c const_ref or a \c const_ptr, this resolves to
//! an equivalent \c const_ref type.

template<typename T>
using ref_cast=typename ref_cast_lookup<T>::type;

//! Convert a \c ref to an equivalent \c ptr.

//! If the template parameter is a \c ref or a \c ptr, this resolves to
//! an equivalent \c ptr type.
//!
//! If the template parameter is a \c const_ref or a \c const_ptr, this resolves to
//! an equivalent \c const_ptr type.

template<typename T>
using ptr_cast=typename ptr_cast_lookup<T>::type;

//! Require an explicit ref or ptr as a function parameter.

//! Helper class that disables implicit/dynamic ref and ptr conversions.
//!
//! \code
//! typedef INSERT_LIBX_NAMESPACE::ref<class1Obj> class1;
//!
//! typedef INSERT_LIBX_NAMESPACE::ref<class2Obj> class2;
//!
//! void foo(const INSERT_LIBX_NAMESPACE::explicit_refptr<class1> &arg)
//! {
//!     const class1 &rarg=arg;
//!
//!     // ...
//! }
//!
//! void foo(const INSERT_LIBX_NAMESPACE::explicit_refptr<class2> &arg)
//! {
//!     const class2 &rarg=arg;
//!
//!     // ...
//! }
//! \endcode
//!
//! refs and ptrs to one type are convertible to refs and ptrs to a different
//! type, and the casting occurs statically, if possible, or dynamicaly.
//! It follows that a function with a ref or a ptr to one type participates
//! in overload resolution when the corresponding parameter is a ref or a ptr
//! to another type, due to an availability of the conversion.
//!
//! Specifying a parameter as a \c explicit_refptr<T> with the template
//! parameter being a ref or a ptr results in the function participating
//! in overload resolution only if the corresponding parameter is a ref or
//! a ptr to the same type.
//!
//! \note
//! Overload resolution still occurs for all variations of ref and ptr
//! as long as the underlying reference-counted class is a same.
//! an \c INSERT_LIBX_NAMESPACE::explicit_refptr<INSERT_LIBX_NAMESPACE::const_ref<classObj>>
//! participates in overload resolution with a
//! \c INSERT_LIBX_NAMESPACE::ref<classObj> parameter, for example. Use
//! \ref explicit_arg "INSERT_LIBX_NAMESPACE::explicit_arg" to disable
//! all overload resolution.

template<typename T>
struct explicit_refptr : T {

	//! Constructor
	template<typename Arg,
		 typename=std::enable_if_t<
			 std::is_same_v<typename refptr_traits<
						std::remove_cvref_t<Arg>
						>::obj_t,
					typename refptr_traits<T>::obj_t>,

			 // This catches const ref or ptr being passed to
			 // a non-const one.
			 decltype((*std::declval<void (*)(const T &)>())
				  (std::declval<const Arg &>()))>>
	explicit_refptr(Arg &&arg)
		: T{std::forward<Arg>(arg)}
	{
	}

	using T::operator<=>;
	using T::operator==;

	//! Inherited ! operator.
	inline bool operator!() const noexcept
	{
		return T::operator!();
	}

	//! Inherited bool operator
	inline operator bool() const noexcept
	{
		return T::operator bool();
	}
};

#if 0
{
#endif
}
#endif
