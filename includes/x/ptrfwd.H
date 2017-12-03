/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_reffwd_H
#define x_reffwd_H

#include <utility>
#include <type_traits>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Create a reference or a nullable pointer reference to a new object.

//! This is the default second parameter to
//! \ref ref "INSERT_LIBX_NAMESPACE::ref" and
//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr", which becomes
//! \ref ref "INSERT_LIBX_NAMESPACE::ref<class>::base" and
//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr<class>::base".
//!
//!
class obj;

class ptrref_base {

public:

	//! Instantiate a new class intance, put it into a reference or a pointer.

	//! \ref ref "INSERT_LIBX_NAMESPACE::ref<class>" and
	//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr<class>" define a %create()
	//! that calls \c %base::factory&lt;TYPE&gt;::%create(), where \c TYPE
	//! is the \c ref or the \c ptr type, with the arguments to
	//! %create() getting forwarded.

	template<typename ptrrefType> class objfactory
	{
	public:

		template<typename... Args_t>
		static inline ptrrefType
		create(Args_t &&...args) LIBCXX_INLINE;
	};

	//! Used by create() to determine if an object implement constructor().

	//! The first parameter is an object class. The second parameter is a
	//! tuple of types of the parameters to create().

	template<typename objClass, typename tuple_type, typename C=void>
	struct has_init {

		//! The object does not have a suitable init().

		static constexpr bool value=false;
	};

	template<typename ...Args> struct create_args;

	//! Specialization that determines if an object implement constructor().

	template<typename objClass, typename ...Args>
	struct has_init<objClass, create_args<Args...>,
			decltype( std::declval<objClass &>().constructor
				  ( std::declval<Args &&>()... ))> {

		//! The object has a suitable init().

		static constexpr bool value=true;
	};
	template<typename ptrrefType, typename args,
		 bool has_init=has_init<typename ptrrefType::obj_type,
					args>::value>
	struct create_and_init;
};

//! The default %create() method.

//! This is a default implementation for the %create() method:
//!
//! \code
//! typedef ptr<widgetObj> widget;
//!
//! widget myWidget=widget::create();
//! \endcode

template<typename ptrrefType>
template<typename... Args_t>
inline ptrrefType ptrref_base::objfactory<ptrrefType>::create(Args_t &&...args)
{
	return create_and_init<ptrrefType,
			       create_args<Args_t...>
			       >::create(std::forward<Args_t>(args)...);
}

//! Define the default base class for a \ref ptr "ptr" or a \ref ref "ref".

//! The default base class is ptrref_base

template<typename>
struct base_type {
public:

	typedef ptrref_base type;
};

template<typename objClass, typename type=typename base_type<objClass>
	 ::type> class ptr;

template<typename objClass, typename type=typename base_type<objClass>
	 ::type> class const_ptr;

template<typename objClass, typename type=typename base_type<objClass>
	 ::type> class ref;

template<typename objClass, typename type=typename base_type<objClass>
	 ::type> class const_ref;

//! Deduction guide for a \ref ptr "ptr".

template<typename objClass> ptr(objClass *) -> ptr<objClass>;

//! Deduction guide for a \ref const_ptr "const_ptr".

template<typename objClass> const_ptr(objClass *) -> const_ptr<objClass>;

//! Deduction guide for a \ref const_ptr "const_ptr".

template<typename objClass> const_ptr(const objClass *) -> const_ptr<objClass>;

//! Deduction guide for a \ref ref "ref".

template<typename objClass> ref(objClass *) -> ref<objClass>;

//! Deduction guide for a \ref const_ref "const_ref".

template<typename objClass> const_ref(objClass *) -> const_ref<objClass>;

//! Deduction guide for a \ref const_ref "const_ref".

template<typename objClass> const_ref(const objClass *) -> const_ref<objClass>;

//! create() and object that does not have a constructor().
template<typename ptrrefType, typename args >
struct ptrref_base::create_and_init<ptrrefType, args, false> {

	template<typename ...Args>
	static inline ptrrefType create(Args && ...argsValue) LIBCXX_INLINE;
};

//! create() and object with a constructor().
template<typename ptrrefType, typename args >
struct ptrref_base::create_and_init<ptrrefType, args, true> {

	template<typename ...Args>
	static inline ptrrefType create(Args && ...argsValue) LIBCXX_INLINE;
};

//! SFINAE-resolved create() for a class without a suitable constructor().

template<typename ptrrefType, typename args >
template<typename ...Args>
inline ptrrefType ptrref_base::create_and_init<ptrrefType, args, false>
::create(Args && ...argsValue)
{
	// Sadly, cannot use uniform initialization syntax here because
	// it warns about narrowing conversion when passing, say a literal
	// 100 constant int to a constructor that takes a size_t parameter.

	auto p=new typename ptrrefType
		::obj_type(std::forward<Args>(argsValue)...);

	p->obj::refadd(1); // Set refcnt's constructor

	return ptrrefType(p);
}

//! SFINAE-resolved create() for a class with a suitable constructor().

template<typename ptrrefType, typename args >
template<typename ...Args>
inline ptrrefType ptrref_base::create_and_init<ptrrefType, args, true>
::create(Args && ...argsValue)
{
	// Sadly, cannot use uniform initialization syntax here because
	// it warns about narrowing conversion when passing, say a literal
	// 100 constant int to a constructor that takes a size_t parameter.
	auto p=new typename ptrrefType
		::obj_type(std::forward<Args>(argsValue)...);

	p->obj::refadd(1); // Set refcnt's constructor

	auto r=ref(p);

	r->constructor(std::forward<Args>(argsValue)...);

	return ptrrefType(r);
}

#if 0
{
#endif
}
#endif
