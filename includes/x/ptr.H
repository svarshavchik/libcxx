/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

/*! \file

This file defines the \c ptr template, a nullable pointer to a reference-counted
object.

*/

#ifndef x_ptr_H
#define x_ptr_H

#include <x/namespace.h>
#include <x/errors.H>
#include <x/ptrfwd.H>
#include <x/weakptrfwd.H>
#include <x/convert.H>
#include <utility>
#include <typeinfo>
#include <functional>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Initialize a constant reference.
//! \see ptr
template<typename objClass, typename baseClass>
inline const_ptr<objClass,baseClass>::const_ptr() noexcept
	: ptrImpl<objClass>(nullptr)
{
}

//! Initialize a constant reference.
//! \see ptr

template<typename objClass, typename baseClass>
inline const_ptr<objClass,baseClass>::
const_ptr(const objClass *pArg) noexcept
	: ptrImpl<objClass>(const_cast<objClass *>(pArg))
{
}

//! Copy constructor

template<typename objClass, typename baseClass>
inline const_ptr<objClass,baseClass>::
const_ptr(const ptrImpl<objClass> &o) noexcept

	: ptrImpl<objClass>(o)
{
}


//! Copy constructor

template<typename objClass, typename baseClass>
template<typename baseClass2>
const_ptr<objClass,baseClass>::
const_ptr(const const_ref<objClass, baseClass2> &o) noexcept
	: ptrImpl<objClass>(o)
{
}

//! Move constructor

template<typename objClass, typename baseClass>
template<typename baseClass2>
const_ptr<objClass,baseClass>::
const_ptr(const_ref<objClass, baseClass2> &&o) noexcept
	: ptrImpl<objClass>(std::move(o))
{
}

//! Copy constructor called by const_ref

template<typename objClass, typename baseClass>
template<typename baseClass2>
const_ptr<objClass,baseClass>::
const_ptr(const ref<objClass, baseClass2> &o) noexcept
	: ptrImpl<objClass>(o)
{
}

//! Move constructor called by const_ref

template<typename objClass, typename baseClass>
template<typename baseClass2>
const_ptr<objClass,baseClass>::
const_ptr(ref<objClass, baseClass2> &&o) noexcept
	: ptrImpl<objClass>(std::move(o))
{
}

//! The default destructor

template<typename objClass, typename baseClass>
inline const_ptr<objClass,baseClass>::~const_ptr()=default;

//! Implement the "*" operator for pointer references

//! Dereferencing results in a constant object.
//!
template<typename objClass, typename baseClass>
inline const objClass &const_ptr<objClass,baseClass>::operator*() const

{
	if (!this->refP)
		null_ptr_deref();

	return *this->refP;
}

//! Implement the "->" operator for pointer references

//! Dereferencing results in a constant object.
//!

template<typename objClass, typename baseClass>
inline const objClass *const_ptr<objClass,baseClass>::operator->() const
{
	if (!this->refP)
		null_ptr_deref();

	return this->refP;
}

//! The assignment operator

template<typename objClass, typename baseClass>
inline const_ptr<objClass, baseClass>
&const_ptr<objClass,baseClass>::operator=(const ptrImpl<objClass> &o)
	noexcept
{
	ptrImpl<objClass>::operator=(o);

	return *this;
}

//! The move operator

template<typename objClass, typename baseClass>
inline const_ptr<objClass, baseClass>
&const_ptr<objClass,baseClass>::operator=(ptrImpl<objClass> &&o)
	noexcept
{
	ptrImpl<objClass>::operator=(std::move(o));

	return *this;
}


//! Forward create() to the base type

template<typename objClass, typename baseClass>
template<typename... Args_t>
inline auto const_ptr<objClass,baseClass>::create(Args_t &&...args)
{
	return baseClass
		::template objfactory< const_ptr<objClass, baseClass> >
		::create(std::forward<Args_t>(args)...);
}

//! Comparison operator

//! std::compare_three_way support total ordering of pointers.

template<typename objClass, typename baseClass>
template<typename T>
inline std::strong_ordering const_ptr<objClass,baseClass>::
operator<=>(const T &o)
	const noexcept requires is_derived_from_ref_or_ptr<T>
{
	typedef common_obj_t<obj_type, typename T::obj_type> *ptr_t;

	return std::compare_three_way::operator()
		(static_cast<ptr_t>(this->refP),
		 static_cast<ptr_t>(o.getrefP()));
}

template<typename objClass, typename baseClass>
template<typename T>
inline bool const_ptr<objClass,baseClass>::
operator==(const T &o)
	const noexcept requires is_derived_from_ref_or_ptr<T>
{
	return operator<=>(o) == std::strong_ordering::equal;
}

template<typename objClass, typename baseClass>
template<typename objClass2>
inline const_ptr<objClass, baseClass>::const_ptr(const ptrImpl<objClass2> &o)
	: ptrImpl<objClass>(nullptr)
{
	operator=(o);
}

//! Convert from a pointer reference to a different type

//! This operator= function allows getting a reference to this
//! object type's superclass or subclass.

template<typename objClass, typename baseClass>
template<typename objClass2> const_ptr<objClass, baseClass>
&const_ptr<objClass, baseClass>::operator=(const ptrImpl<objClass2> &o)
{
	objClass *newRefP=0;

	if (o.refP)
	{
		newRefP=convert<objClass2, objClass, false>::cast(o.refP);
		if (newRefP == 0)
			invalid_cast(typeid(*o.refP).name(),
				     typeid(objClass).name());
	}

	this->setRef_noconscheck(newRefP);
	return *this;
}

template<typename objClass, typename baseClass>
template<typename objClass2> const_ptr<objClass, baseClass>
&const_ptr<objClass, baseClass>::operator=(const objClass2 *o)
{
	objClass *newRefP=0;

	if (o)
	{
		newRefP=const_cast<objClass *>
			(convert<objClass2, objClass, true>::cast(o));
		if (newRefP == 0)
			invalid_cast(typeid(*o).name(),
				     typeid(objClass).name());
	}

	this->setRef(newRefP);
	return *this;
}

template<typename objClass, typename baseClass>
template<typename baseClass2>
inline const_ptr<objClass, baseClass>
::const_ptr(const_ptr<objClass, baseClass2> &&o)
	noexcept
	: ptrImpl<objClass>(std::move(o))
{
}

template<typename objClass, typename baseClass>
template<typename baseClass2>
inline const_ptr<objClass, baseClass>::const_ptr(ptr<objClass, baseClass2> &&o)
	noexcept
	: ptrImpl<objClass>(std::move(o))
{
}

//! Default constructor

template<typename objClass, typename baseClass>
inline  ptr<objClass, baseClass>::ptr() noexcept
: const_ptr<objClass, baseClass>(nullptr)
{
}

//! The default constructor from \c this

//! Allows construction of a \c ptr from \c this.

template<typename objClass, typename baseClass>
inline ptr<objClass,baseClass>::ptr(objClass *pArg) noexcept
	: const_ptr<objClass, baseClass>(pArg)
{
}

//! Copy constructor

template<typename objClass, typename baseClass>
inline ptr<objClass,baseClass>::ptr(const ptr<objClass, baseClass> &o) noexcept
	: const_ptr<objClass, baseClass>(o)
{
}

//! Move constructor

template<typename objClass, typename baseClass>
inline ptr<objClass,baseClass>::ptr(ptr<objClass, baseClass> &&o) noexcept
	: const_ptr<objClass, baseClass>(std::move(o))
{
}

//! Copy constructor

template<typename objClass, typename baseClass>
template<typename baseClass2>
ptr<objClass,baseClass>::ptr(const ref<objClass, baseClass2> &o) noexcept
	: const_ptr<objClass, baseClass>(o)
{
}

//! Move constructor

template<typename objClass, typename baseClass>
template<typename baseClass2>
ptr<objClass,baseClass>::ptr(ref<objClass, baseClass2> && o) noexcept
	: const_ptr<objClass, baseClass>(std::move(o))
{
}

//! Copy constructor called by const_ref

template<typename objClass, typename baseClass>
template<typename baseClass2>
ptr<objClass,baseClass>::ptr(const const_ref<objClass, baseClass2> &o)
	noexcept
	: const_ptr<objClass, baseClass>(o)
{
}

//! Move constructor called by const_ref

template<typename objClass, typename baseClass>
template<typename baseClass2>
ptr<objClass,baseClass>::ptr(const_ref<objClass, baseClass2> && o) noexcept
	: const_ptr<objClass, baseClass>(std::move(o))
{
}

//! The default destructor

//! If this is the last reference bound to an object, the object
//! gets automatically destroyed via \c delete.

template<typename objClass, typename baseClass>
inline ptr<objClass,baseClass>::~ptr()=default;

//! Implement the "*" operator for object references

//! Using "*" on a reference works as expected, as if the reference
//! was a native pointer.
//! An exception gets thrown if this reference is not bound to an
//! instance of \c objClass.
template<typename objClass, typename baseClass>
inline objClass &ptr<objClass,baseClass>::operator*() const

{
	if (!this->refP)
		null_ptr_deref();

	return const_cast<objClass &>(*this->refP);
}

//! Implement the "->" operator for object references

//! Using "->" on a reference works as expected, as if the reference
//! was a native pointer.
//! An exception gets thrown if this reference is not bound to an
//! instance of \c objClass.
template<typename objClass, typename baseClass>
inline objClass *ptr<objClass,baseClass>::operator->() const

{
	if (!this->refP)
		null_ptr_deref();

	return const_cast<objClass *>(this->refP);
}

//! The assignment operator

//! If this reference is bound to an object, its reference count
//! gets decremented, and if this is its last reference, the object
//! gets destroyed via \c delete. If \c o is bound to an instance of
//! \c objClass, its reference count gets incremented.

template<typename objClass, typename baseClass>
inline ptr<objClass, baseClass> &ptr<objClass,baseClass>
::operator=(//! An existing reference.

	    //! If this reference is bound to an instance of \c objClass,
	    //! its reference count gets incremented.

	   const ptr<objClass, baseClass> &o)
	noexcept
{
	const_ptr<objClass, baseClass>::operator=(o);

	return *this;
}

//! The move operator

template<typename objClass, typename baseClass>
inline ptr<objClass, baseClass> &ptr<objClass,baseClass>
::operator=(//! An existing reference.

	    //! If this reference is bound to an instance of \c objClass,
	    //! its reference count gets incremented.

	    ptr<objClass, baseClass> &&o)
	noexcept
{
	const_ptr<objClass, baseClass>::operator=(std::move(o));

	return *this;
}


//! Convert from a nullptr
template<typename objClass, typename baseClass>
inline ptr<objClass, baseClass>
&ptr<objClass,baseClass>::operator=(std::nullptr_t)
{
	return operator=((objClass *)0);
}

//! Make a weak pointer out of me.

template<typename objClass, typename baseClass>
auto ptr<objClass,baseClass>::weaken() const
{
	return weakptr<ptr<objClass, baseClass>>(*this);
}











//! Construct from a reference to a subclass or a superclass.

//! \overload
//! This constructor allows getting a reference to this
//! object type's superclass or subclass.

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2> inline
ptr<objClass, baseClass>::ptr(const ptr<objClass2, baseClass2> &o)

	: const_ptr<objClass, baseClass>(nullptr)
{
	operator=(o);
}

//! Move constructor from a subclass or a superclass.

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2> inline
ptr<objClass, baseClass>::ptr(ptr<objClass2, baseClass2> &&o)

	: const_ptr<objClass, baseClass>(nullptr)
{
	operator=(std::forward<ptr<objClass2, baseClass2> >(o));
}

//! Convert from a reference to a different type.

//! This operator= function allows getting a reference to this
//! object type's superclass or subclass.
template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
ptr<objClass, baseClass>
&ptr<objClass, baseClass>::operator=(const ptr<objClass2, baseClass2> &o)

{
	const_ptr<objClass, baseClass>::operator=(o);
	return *this;
}

//! Move from a reference to a different type.

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
ptr<objClass, baseClass>
&ptr<objClass, baseClass>::operator=(ptr<objClass2, baseClass2> &&o)
{
	const_ptr<objClass, baseClass>::operator=(std::move(o));
	return *this;
}

//! Convert from a reference to a different type.

//! This operator= function allows getting a reference to this
//! object type's superclass or subclass.

template<typename objClass, typename baseClass>
template<typename objClass2>
ptr<objClass, baseClass>
&ptr<objClass, baseClass>::operator=(objClass2 *o)

{
	const_ptr<objClass, baseClass>::operator=(o);
	return *this;
}

//! Forward create() to the base type.

template<typename objClass, typename baseClass>
template<typename... Args_t>
inline auto ptr<objClass, baseClass>::create(Args_t &&...args)
{
	return baseClass
		::template objfactory< ptr<objClass, baseClass> >
		::create(std::forward<Args_t>(args)...);
}

#if 0
{
#endif
}
#endif
