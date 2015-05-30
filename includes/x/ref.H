/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ref_H
#define x_ref_H

#include <x/ptr.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A reference to a constant object.

//! This is a version of \ref ref "ref"
//! that refers to a constant object.
//! See \ref ref "ref" for more information. With the sole %exception being that
//! dereferencing gives a constant object,
//! \ref const_ref "const_ref" is identical in all
//! other respects to \ref ref "ref". This is similar to the relationship
//! between an \c iterator and a \c const_iterator in the C++ Standard
//! Template library.
//!
//! And similar to STL iterators,
//! \c ref<objClass> can be converted to \c const_ref<objClass>, but not the
//! other
//! way around.

template<typename objClass, typename baseClass> class const_ref {

protected:
	//! The underlying pointer object.

	ptr<objClass, baseClass> p;

public:
	template<typename objClass2>
	friend class ptrImpl;

	//! Define this pointer's base class.

	typedef baseClass base;

	//! Define this pointer's object class.

	typedef objClass obj_type;

	//! Initialize a constant reference.
	//! \see ref

	inline explicit
	const_ref(const objClass *pArg) noexcept
		__attribute__((artificial))
		: p(const_cast<objClass *>(pArg))
	{
		if (p.null())
			null_ptr_deref();
	}

	//! The default destructor

	inline ~const_ref() noexcept __attribute__((artificial))
	{
	}

	//! Implement the "*" operator for object references

	//! Dereferencing results in a constant object.
	//!
	inline const objClass &operator*() const
		__attribute__((artificial))
	{
		return *this->p.refP;
	}

	//! Implement the "->" operator for object references

	//! Dereferencing results in a constant object.
	//!

	inline const objClass *operator->() const
		__attribute__((artificial))
	{
		return this->p.refP;
	}

	//! The assignment operator

	inline const_ref<objClass, baseClass>
	&operator=(const ptrImpl<objClass> &o)
		noexcept __attribute__((artificial))
	{
		if (o.null())
			null_ptr_deref();

		ptrImpl<objClass> &r=this->p;

		r.operator=(o);

		return *this;
	}

	//! The move operator

	inline const_ref<objClass, baseClass>
	&operator=(ptrImpl<objClass> &&o)
		noexcept __attribute__((artificial))
	{
		if (o.null())
			null_ptr_deref();

		objClass *q=this->p.refP;

		this->p.refP=o.refP;

		o.refP=q;

		return *this;
	}

	//! The assignment operator

	//! \overload

	inline const_ref<objClass, baseClass>
	&operator=(//! An existing reference.

		   //! If this reference is bound to an instance of \c objClass,
		   //! its reference count gets incremented.

		   const const_ref<objClass, baseClass> &o)
		noexcept __attribute__((artificial))
	{
		p=o.p;
		return *this;
	}

	//! The move operator

	//! \overload

	inline const_ref<objClass, baseClass>
	&operator=(//! An existing reference.

		   //! If this reference is bound to an instance of \c objClass,
		   //! its reference count gets incremented.

		   const_ref<objClass, baseClass> &&o)
		noexcept __attribute__((artificial))
	{
		objClass *q=this->p.refP;
		this->p.refP=o.p.refP;
		o.p.refP=q;

		return *this;
	}

	//! A ref of one type is friends with refs of all other types.
	template<typename objClass2, typename baseClass2>
	friend class const_ref;

	//! Copy constructor

	inline explicit
	const_ref(const const_ref<objClass, baseClass> &o) noexcept
		__attribute__((artificial))
		: p(o)
	{
	}

	//! Move constructor

	inline explicit
	const_ref(const_ref<objClass, baseClass> &&o) noexcept
		__attribute__((artificial))
		: p(std::move(o))
	{
	}

	//! Construct from a reference to a subclass or a superclass

	template<typename objClass2>
	inline const_ref(const ptrImpl<objClass2> &o)
		__attribute__((artificial));

	//! Move from a reference to a subclass or a superclass

	template<typename objClass2>
	inline const_ref(ptrImpl<objClass2> &&o)
		__attribute__((artificial));

	//! Construct from a reference to a subclass or a superclass

	template<typename objClass2, typename baseClass2>
	inline const_ref(const const_ref<objClass2, baseClass2> &o)
		__attribute__((artificial));

	//! Move from a reference to a subclass or a superclass

	template<typename objClass2, typename baseClass2>
	inline const_ref(const_ref<objClass2, baseClass2> &&o)
		__attribute__((artificial));

	//! Convert from a reference to a different type

	//! This operator= function allows getting a reference to this
	//! object type's superclass or subclass.
	template<typename objClass2> inline const_ref<objClass, baseClass>
	&operator=(const ptrImpl<objClass2> &o)
		__attribute__((artificial));

	//! Convert this reference to a pointer

	operator const const_ptr<objClass, baseClass> &() const noexcept
	{
		return this->p;
	}

	//! Convert this constant reference to a pointer to one of its subclasses or superclasses.

	template<typename objClass2, typename baseClass2>
	inline operator const const_ptr<objClass2, baseClass2>() const
	{
		return this->p;
	}

	//! Comparison operator

	//! C++11 allows pointer comparisons.
	template<typename objClass2, typename baseClass2>
	inline bool operator==(const const_ptr<objClass2, baseClass2> &o)
		const noexcept
	{
		return p == o;
	}

	//! C++11 allows pointer comparisons.
	template<typename objClass2, typename baseClass2>
	inline bool operator!=(const const_ptr<objClass2, baseClass2> &o)
		const noexcept
	{
		return p != o;
	}

	//! Comparison operator

	//! C++11 allows pointer comparisons.
	template<typename objClass2, typename baseClass2>
	inline bool operator==(const const_ref<objClass2, baseClass2> &o)
		const noexcept
	{
		return p == o;
	}

	//! C++11 allows pointer comparisons.
	template<typename objClass2, typename baseClass2>
	inline bool operator!=(const const_ref<objClass2, baseClass2> &o)
		const noexcept
	{
		return p != o;
	}

	//! Forward create() to the base type

	template<typename... Args_t>
	static inline auto create(Args_t &&...args)
	{
		return baseClass
			::template objfactory< ref<objClass, baseClass> >
			::create(std::forward<Args_t>(args)...);
	}
};


template<typename objClass, typename baseClass>
template<typename objClass2>
inline const_ref<objClass, baseClass>::const_ref(const ptrImpl<objClass2> &o)

{
	static_cast<const_ptr<objClass, baseClass> &>(p)=o;

	if (p.null())
		null_ptr_deref();
}

template<typename objClass, typename baseClass>
template<typename objClass2>
inline const_ref<objClass, baseClass>::const_ref(ptrImpl<objClass2> &&o)

{
	static_cast<const_ptr<objClass, baseClass> &>(p)=std::move(o);

	if (p.null())
		null_ptr_deref();
}

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline const_ref<objClass, baseClass>::const_ref(const const_ref<objClass2, baseClass2> &o)

{
	p.operator=(o.p);
}

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline const_ref<objClass, baseClass>::const_ref(const_ref<objClass2, baseClass2> &&o)

{
	p.operator=(std::move(o.p));
}

template<typename objClass, typename baseClass>
template<typename objClass2> inline const_ref<objClass, baseClass>
&const_ref<objClass, baseClass>::operator=(const ptrImpl<objClass2> &o)

{
	*this=const_ref<objClass, baseClass>(o);
	return *this;
}

//! A non-null pointer reference.

//! This template defines a pointer to a reference-counted object, and is
//! similar to \ref ptr "INSERT_LIBX_NAMESPACE::ptr", except that
//! a \ref ref "INSERT_LIBX_NAMESPACE::ref" must always refer to an instance
//! of the referenced object class.
//!
//! A \ref ptr "INSERT_LIBX_NAMESPACE::ptr" can be unbound, an equivalent of
//! a native NULL pointer. Dereferencing a NULL
//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr", using its \c * or \c -> operator,
//! throws an %exception. This
//! \ref ref "INSERT_LIBX_NAMESPACE::ref" also has a \c * and a \c -> operators,
//! which work exactly like their
//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr" counterparts, except that they
//! never throw an %exception because
//! a \ref ptr "INSERT_LIBX_NAMESPACE::ptr" can never be a NULL pointer.
//! This is similar to a relationship between a native C++ pointer and a
//! reference.
//!
//! There's also a \ref const_ref "INSERT_LIBX_NAMESPACE::const_ref", which
//! is the analogue for  \ref const_ptr "INSERT_LIBX_NAMESPACE::const_ptr",
//! and carries the similar semantisc.
//!
//! A \ref ref "INSERT_LIBX_NAMESPACE::ref" is generally interchangeable with
//! a \ref ptr "INSERT_LIBX_NAMESPACE::ptr". Each one of them can be
//! converted to the other. Converting or assigning a
//! \ref ptr "INSERT_LIBX_NAMESPACE::ptr" to a
//! \ref ref "INSERT_LIBX_NAMESPACE::ref" immediately throws an
//! %exception if the \ref ptr "INSERT_LIBX_NAMESPACE::ptr" is NULL. The
//! %exception gets thrown immediately, when the assignment, or a conversion,
//! takes place, and not when
//! \ref ref "INSERT_LIBX_NAMESPACE::ref"'s \c * or \c -> operator is used
//! at some pointer later.
//!
//! \ref const_ref "INSERT_LIBX_NAMESPACE::const_ref" is the equivalent of
//! \ref const_ptr "INSERT_LIBX_NAMESPACE::const_ptr".

template<typename objClass, typename baseClass>
class ref : public const_ref<objClass, baseClass> {
public:
	template<typename objClass2>
	friend class ptrImpl;

	//! Construct for a pointer to an existing instance of the object.

	inline explicit ref(//! Existing object instance. This pointer cannot be NULL.
			    objClass *pArg) noexcept
		__attribute__((artificial))
		: const_ref<objClass, baseClass>(pArg)
	{
	}

	//! Copy constructor

	inline ref(const ref<objClass, baseClass> &o) noexcept
		__attribute__((artificial))
		: const_ref<objClass, baseClass>(o)
	{
	}

	//! Move constructor

	inline ref(ref<objClass, baseClass> &&o) noexcept
		__attribute__((artificial))
		: const_ref<objClass, baseClass>(std::move(o))
	{
	}

	//! The default destructor

	//! If this is the last pointer to an object, the object
	//! gets automatically destroyed via \c delete.

	inline ~ref() noexcept __attribute__((artificial))
	{
	}

	//! The "*" operator.

	inline objClass &operator*() const noexcept
		 __attribute__((artificial))
	{
		return *this->p.refP;
	}

	//! The "->" operator.

	inline objClass *operator->() const noexcept
		__attribute__((artificial))
	{
		return this->p.refP;
	}

	//! Assign from an existing nullable pointer.

	inline ref<objClass, baseClass>
	&operator=(//! An existing pointer. An exception gets thrown if this pointer is NULL.
		   const ptr<objClass, baseClass> &o)
		noexcept __attribute__((artificial))
	{
		const_ref<objClass, baseClass>::operator=(o);

		return *this;
	}

	//! Move from an existing nullable pointer.

	inline ref<objClass, baseClass>
	&operator=(//! An existing pointer. An exception gets thrown if this pointer is NULL.
		   ptr<objClass, baseClass> &&o)
		noexcept __attribute__((artificial))
	{
		const_ref<objClass, baseClass>::operator=(std::move(o));

		return *this;
	}

	//! The assignment operator.

	inline ref<objClass, baseClass>
	&operator=(//! An existing reference.

		   const ref<objClass, baseClass> &o)
		noexcept __attribute__((artificial))
	{
		const_ref<objClass, baseClass>::operator=(static_cast<const const_ref<objClass, baseClass> &>(o));

		return *this;
	}

	//! The move operator

	inline ref<objClass, baseClass>
	&operator=(//! An existing reference.

		   ref<objClass, baseClass> &&o)
		noexcept __attribute__((artificial))
	{
		const_ref<objClass, baseClass>::operator=(std::move(static_cast<const const_ref<objClass, baseClass> &>(o)));

		return *this;
	}

	using const_ref<objClass, baseClass>::operator==;
	using const_ref<objClass, baseClass>::operator!=;

	template<typename objClass2, typename baseClass2> friend class ref;

	//! Construct from a pointer to a superclass or a subclass of the referenced object class.

	template<typename objClass2, typename baseClass2> inline
	ref(//! A reference to another subclass or a superclass of this object.

	    //! An exception gets thrown if this pointer is unbound, a NULL
	    //! pointer, or if the referenced object cannot be converted to
	    //! this pointer's object class.

	    const ptr<objClass2, baseClass2> &o)
		__attribute__((artificial));

	//! Move from a pointer to a superclass or a subclass of the referenced object class.

	template<typename objClass2, typename baseClass2> inline
	ref(//! A reference to another subclass or a superclass of this object.

	    //! An exception gets thrown if this pointer is unbound, a NULL
	    //! pointer, or if the referenced object cannot be converted to
	    //! this pointer's object class.

	    ptr<objClass2, baseClass2> &&o)
		__attribute__((artificial));

	//! Construct from a reference to a superclass or a subclass of the object class.

	template<typename objClass2, typename baseClass2> inline
	ref(//! A reference to another subclass or a superclass of this object class.

	    //! An exception gets thrown if it cannot be converted to the right
	    //! object type.
	    const ref<objClass2, baseClass2> &o)
		__attribute__((artificial));

	//! Move from a reference to a superclass or a subclass of the object class.

	template<typename objClass2, typename baseClass2> inline
	ref(//! A reference to another subclass or a superclass of this object class.

	    //! An exception gets thrown if it cannot be converted to the right
	    //! object type.
	    ref<objClass2, baseClass2> &&o)
		__attribute__((artificial));

	//! Assign from a pointer to a different type.

	template<typename objClass2, typename baseClass2>
	inline ref<objClass, baseClass>
	&operator=(//! Reference to this object's superclass or subclass instance.

		   //! An exception gets thrown if this pointer is unbound,
		   //! a NULL
		   //! pointer, or if the referenced object cannot be converted to
		   //! this pointer's object class.
		   const ptr<objClass2, baseClass2> &o)
		__attribute__((artificial));

	//! Move from a pointer to a different type.

	template<typename objClass2, typename baseClass2>
	inline ref<objClass, baseClass>
	&operator=(//! Reference to this object's superclass or subclass instance.

		   //! An exception gets thrown if this pointer is unbound,
		   //! a NULL
		   //! pointer, or if the referenced object cannot be converted to
		   //! this pointer's object class.
		   ptr<objClass2, baseClass2> &&o)
		__attribute__((artificial));

	//! Assign from a reference to a different type.

	template<typename objClass2, typename baseClass2>
	inline ref<objClass, baseClass>
	&operator=(//! Reference to this object's superclass or subclass instance.

		   //! An exception gets thrown if the referenced objec cannot
		   //! be converted to this pointer's object class.
		   const ref<objClass2, baseClass2> &o)
		__attribute__((artificial));

	//! Move from a reference to a different type.

	template<typename objClass2, typename baseClass2>
	inline ref<objClass, baseClass>
	&operator=(//! Reference to this object's superclass or subclass instance.

		   //! An exception gets thrown if the referenced objec cannot
		   //! be converted to this pointer's object class.
		   ref<objClass2, baseClass2> &&o)
		__attribute__((artificial));

	//! Convert from a native pointer to a different type.

	template<typename objClass2> inline ref<objClass, baseClass>
	&operator=(objClass2 *o)
		__attribute__((artificial));

	//! Convert this reference to a pointer

	inline operator const ptr<objClass, baseClass> &() const noexcept
	{
		return this->p;
	}

	//! Convert this reference to a pointer

	inline operator const const_ptr<objClass, baseClass> &() const noexcept
	{
		return this->p;
	}

	//! Convert this reference to a pointer to one of its subclasses or superclasses.

	template<typename objClass2, typename baseClass2>
	inline operator const ptr<objClass2, baseClass2>() const
	{
		return this->p;
	}

	//! Convert this reference to a pointer to one of its subclasses or superclasses.

	template<typename objClass2, typename baseClass2>
	inline operator const const_ptr<objClass2, baseClass2>() const
	{
		return this->p;
	}
};

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
inline ref<objClass, baseClass>::ref(const ref<objClass2, baseClass2> &o)
	: const_ref<objClass, baseClass>(o)
{
}

template<typename objClass, typename baseClass>
template<typename objClass2, typename baseClass2>
inline ref<objClass, baseClass>::ref(ref<objClass2, baseClass2> &&o)
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

#if 0
{
#endif
}
#endif