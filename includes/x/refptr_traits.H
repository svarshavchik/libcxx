/*
** Copyright 2016 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_refptr_traits_H
#define x_refptr_traits_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

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
	 typename baseClass>
class refptr_traits_base {

public:

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

//! The traits of a reference or a pointer object.

template<typename T> class refptr_traits;

//! Define traits for a \ref ref "INSERT_LIBX_NAMESPACE::ref".

//! \see refptr_traits_base
template<typename objClass, typename baseClass>
class refptr_traits<ref<objClass, baseClass>>
	: public refptr_traits_base<objClass, baseClass> {};

//! Define traits for a \ref ptr "INSERT_LIBX_NAMESPACE::ref".

//! \see refptr_traits_base
template<typename objClass, typename baseClass>
class refptr_traits<ptr<objClass, baseClass>>
	: public refptr_traits_base<objClass, baseClass> {};

//! Define traits for a \ref const_ref "INSERT_LIBX_NAMESPACE::ref".

//! \see refptr_traits_base
template<typename objClass, typename baseClass>
class refptr_traits<const_ref<objClass, baseClass>>
	: public refptr_traits_base<objClass, baseClass> {};

//! Define traits for a \ref const_ptr "INSERT_LIBX_NAMESPACE::ref".

//! \see refptr_traits_base
template<typename objClass, typename baseClass>
class refptr_traits<const_ptr<objClass, baseClass>>
	: public refptr_traits_base<objClass, baseClass> {};


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

#if 0
{
#endif
}
#endif
