/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ref_H
#define x_ref_H

#include <x/ptr.H>
#include <x/namespace.h>
#include <utility>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Initialize a constant reference from "this".

//! \see ref

template<typename objClass, typename baseClass>
inline const_ref<objClass, baseClass>
::const_ref(const objClass *pArg)
	: p(const_cast<objClass *>(pArg))
{
	if (p.null())
		null_ptr_deref();
}

//! Copy constructor

template<typename objClass, typename baseClass>
inline const_ref<objClass, baseClass>
::const_ref(const const_ref<objClass, baseClass> &o) noexcept
	: p(o)
{
}

////////////////////////////////////////////////////////////////////////
//
// Assignment operators of const_ref

//! The assignment operator from a nullable pointer reference.

template<typename objClass, typename baseClass>
inline const_ref<objClass, baseClass>
&const_ref<objClass, baseClass>::operator=(const ptrImpl<objClass> &o)
{
	if (o.null())
		null_ptr_deref();

	ptrImpl<objClass> &r=this->p;

	r.operator=(o);

	return *this;
}

//! Assignment from a nullable ptr to a different class.

template<typename objClass, typename baseClass>
template<typename objClass2> inline const_ref<objClass, baseClass>
&const_ref<objClass, baseClass>::operator=(const ptrImpl<objClass2> &o)

{
	*this=const_ref<objClass, baseClass>(o);
	return *this;
}

////////////////////////////////////////////////////////////////////////
//
// Move operators of const_ref

//! The move operator from a nullable pointer reference.

template<typename objClass, typename baseClass>
inline const_ref<objClass, baseClass>
&const_ref<objClass, baseClass>::operator=(ptrImpl<objClass> &&o)
{
	if (o.null())
		null_ptr_deref();

	std::swap(p.refP, o.refP);

	return *this;
}

//! The move operator

//! \overload

template<typename objClass, typename baseClass>
inline const_ref<objClass, baseClass>
&const_ref<objClass, baseClass>
::operator=(const_ref<objClass, baseClass> &&o)
	noexcept
{
	std::swap(p.refP, o.p.refP);
	return *this;
}

//! Destructor

template<typename objClass, typename baseClass>
inline const_ref<objClass, baseClass>::~const_ref()=default;

//! Implement the "*" operator for object references

//! Dereferencing results in a constant object.
//!
template<typename objClass, typename baseClass>
inline const objClass &const_ref<objClass, baseClass>::operator*() const
{
	return *this->p.refP;
}

//! Implement the "->" operator for object references

//! Dereferencing results in a constant object.
//!

template<typename objClass, typename baseClass>
inline const objClass *const_ref<objClass, baseClass>::operator->() const
{
	return this->p.refP;
}


//! Convert this reference to a pointer

template<typename objClass, typename baseClass>
inline const_ref<objClass, baseClass>
::operator const const_ptr<objClass, baseClass> &() const noexcept
{
	return this->p;
}

//! Comparison operator

//! std::compare_three_way support total ordering of pointers.

template<typename objClass, typename baseClass>
template<typename T>
inline std::strong_ordering const_ref<objClass,baseClass>::
operator<=>(const T &o)
	const noexcept requires is_derived_from_ref_or_ptr<T>
{
	typedef common_obj_t<obj_type, typename T::obj_type> *ptr_t;

	return p.std::compare_three_way::operator()
		(static_cast<ptr_t>(this->p.refP),
		 static_cast<ptr_t>(o.getrefP()));
}

//! Comparison operator

//! std::compare_three_way support total ordering of pointers.

template<typename objClass, typename baseClass>
template<typename T>
inline bool const_ref<objClass,baseClass>::
operator==(const T &o)
	const noexcept requires is_derived_from_ref_or_ptr<T>
{
	return operator<=>(o) == std::strong_ordering::equal;
}

//! Forward create() to the base type

template<typename objClass, typename baseClass>
template<typename... Args_t>
inline auto const_ref<objClass, baseClass>::create(Args_t &&...args)
{
	return baseClass
		::template objfactory< const_ref<objClass, baseClass> >
		::create(std::forward<Args_t>(args)...);
}

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline const_ref<objClass, baseClass>
::operator const const_ptr<objClass2, baseClass2>() const
{
	return this->p;
}


//////////////////////////////////////////////////////////////////////////////

//! Constructor for a pointer to an existing instance of the object.

template<typename objClass, typename baseClass>
inline ref<objClass, baseClass>
::ref(//! Existing object instance. This pointer cannot be NULL.
		    objClass *pArg) noexcept

	: const_ref<objClass, baseClass>(pArg)
{
}

//! Copy constructor

template<typename objClass, typename baseClass>
inline ref<objClass, baseClass>
::ref(const ref<objClass, baseClass> &o) noexcept
	: const_ref<objClass, baseClass>(o)
{
}


//! The destructor

//! If this is the last pointer to an object, the object
//! gets automatically destroyed via \c delete.

template<typename objClass, typename baseClass>
inline ref<objClass, baseClass>::~ref()=default;

//! The "*" operator.

template<typename objClass, typename baseClass>
inline objClass &ref<objClass, baseClass>
::operator*() const noexcept

{
	return *this->p.refP;
}

//! The "->" operator.

template<typename objClass, typename baseClass>
inline objClass *ref<objClass, baseClass>
::operator->() const noexcept

{
	return this->p.refP;
}

//! Assign from an existing nullable pointer.

template<typename objClass, typename baseClass>
inline ref<objClass, baseClass> &ref<objClass, baseClass>
::operator=(//! An existing pointer. An exception gets thrown if this pointer is NULL.
	   const ptr<objClass, baseClass> &o)
	noexcept
{
	const_ref<objClass, baseClass>::operator=(o);

	return *this;
}

//! Move from an existing nullable pointer.

template<typename objClass, typename baseClass>
inline ref<objClass, baseClass> &ref<objClass, baseClass>
::operator=(//! An existing pointer. An exception gets thrown if this pointer is NULL.
	   ptr<objClass, baseClass> &&o)
	noexcept
{
	const_ref<objClass, baseClass>::operator=(std::move(o));

	return *this;
}

//! The assignment operator.

template<typename objClass, typename baseClass>
inline ref<objClass, baseClass> &ref<objClass, baseClass>
::operator=(//! An existing reference.
	    const ref<objClass, baseClass> &o)
	noexcept
{
	const_ref<objClass, baseClass>::operator=(static_cast<const const_ref<objClass, baseClass> &>(o));

	return *this;
}

//! The move operator

template<typename objClass, typename baseClass>
inline ref<objClass, baseClass> &ref<objClass, baseClass>
::operator=(//! An existing reference.

	   ref<objClass, baseClass> &&o)
	noexcept
{
	const_ref<objClass, baseClass>::operator=(std::move(o));

	return *this;
}

//! Convert this reference to a pointer

template<typename objClass, typename baseClass>
inline ref<objClass, baseClass>
::operator const ptr<objClass, baseClass> &() const noexcept
{
	return this->p;
}

//! Convert this reference to a pointer

template<typename objClass, typename baseClass>
inline ref<objClass, baseClass>
::operator const const_ptr<objClass, baseClass> &() const noexcept
{
	return this->p;
}

//! Convert this reference to a pointer to one of its subclasses or superclasses.

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline ref<objClass, baseClass>
::operator const ptr<objClass2, baseClass2>() const
{
	return this->p;
}

//! Convert this reference to a pointer to one of its subclasses or superclasses.

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline ref<objClass, baseClass>
::operator const const_ptr<objClass2, baseClass2>() const
{
	return this->p;
}

//! Make a weak pointer.

template<typename objClass, typename baseClass>
inline auto ref<objClass, baseClass>::weaken() const
{
	return this->p.weaken();
}

//! Copy constructor

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline ref<objClass, baseClass>::ref(const ptr<objClass2, baseClass2> &o)
	: const_ref<objClass, baseClass>(o)
{
}

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline ref<objClass, baseClass>::ref(ptr<objClass2, baseClass2> &&o)
	: const_ref<objClass, baseClass>(std::move(o))
{
}

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline ref<objClass, baseClass> &
ref<objClass, baseClass>::operator=(const ptr<objClass2, baseClass2> &o)
{
	const_ref<objClass, baseClass>::operator=(o);
	return *this;
}

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline ref<objClass, baseClass> &
ref<objClass, baseClass>::operator=(ptr<objClass2, baseClass2> &&o)
{
	const_ref<objClass, baseClass>::operator=(std::move(o));
	return *this;
}

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline ref<objClass, baseClass> &
ref<objClass, baseClass>::operator=(const ref<objClass2, baseClass2> &o)
{
	const_ref<objClass, baseClass>
		::operator=(static_cast<const const_ref<objClass2,
							baseClass2> &>(o));
	return *this;
}

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline ref<objClass, baseClass> &
ref<objClass, baseClass>::operator=(ref<objClass2, baseClass2> &&o)

{
	const_ref<objClass, baseClass>
		::operator=(std::move(static_cast<const const_ref<objClass2,
								  baseClass2> &>
				      (o)));
	return *this;
}

template<typename objClass, typename baseClass>
template<typename objClass2>
inline ref<objClass, baseClass> &
ref<objClass, baseClass>::operator=(objClass2 *o)
{
	const_ref<objClass, baseClass>::operator=(o);
	return *this;
}

template<typename objClass, typename baseClass>
template<typename... Args_t>
inline auto ref<objClass, baseClass>::create(Args_t &&...args)
{
	return baseClass
		::template objfactory< ref<objClass, baseClass> >
		::create(std::forward<Args_t>(args)...);
}

#define x_ref_H_included

#include <x/ref_obj.H>
#include <x/ref_obj_ondestroy.H>

#if 0
{
#endif
}
#endif
