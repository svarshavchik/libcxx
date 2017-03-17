/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ptrimpl_H
#define x_ptrimpl_H

#include <x/refcnt.H>
#include <x/ptrfwd.H>
#include <x/namespace.h>

#include <typeinfo>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Reference pointer implementation

//! This class stores the pointer to the underlying reference-counted object,
//! and implements its housekeeping functions.

template<typename objClass> class ptrImpl {

public:
	//! Object that this reference is bound to

	//! \internal
	//! This pointer is \c NULL when this reference is not bound to
	//! any object.
	objClass *refP;

	//! Check if this is an unbound reference

	//! \return \c true, if this reference is not bound to an instance,
	//! \c false otherwise.

	inline bool null() const noexcept __attribute__((artificial))
	{
		return refP == 0;
	}

	//! Make it work in boolean context
	explicit operator bool() const noexcept
	{
		return !null();
	}

	//! Make it work in boolean context
	bool operator!() const noexcept
	{
		return null();
	}
	//! Define this reference's base type

	typedef objClass obj_type;

	friend class obj;

	//! Default constructor

	ptrImpl(//! If pArg is not NULL, the object's reference count gets incremented.
		//! When pArg is NULL, this is an unbound reference.
		objClass *pArg) noexcept;

	//! Default destructor
	~ptrImpl();

	//! The default copy constructor

	inline ptrImpl(//! An existing reference.

		       //! If this reference is bound to an instance of
		       //! \c objClass, its reference count gets incremented.
		       const ptrImpl<objClass> &o ) noexcept;

	//! Copy constructor from a known non-NULL reference

	template<typename baseClass>
	inline explicit
	ptrImpl(const const_ref<objClass, baseClass> &o) noexcept
		: refP(o.p.refP)
	{
		refP->refcnt::refadd(1);
	}

	//! Copy constructor from a known non-NULL reference

	template<typename baseClass>
	inline explicit
	ptrImpl(const ref<objClass, baseClass> &o) noexcept
		: refP(o.p.refP)
	{
		refP->refcnt::refadd(1);
	}

	//! Move constructor from a known non-NULL reference

	template<typename baseClass>
	inline explicit
	ptrImpl(const_ref<objClass, baseClass> &&o) noexcept
		: refP(o.p.refP)
	{
		refP->refcnt::refadd(1);
	}

	//! Move constructor from a known non-NULL reference

	template<typename baseClass>
	inline explicit
	ptrImpl(ref<objClass, baseClass> &&o) noexcept
		: refP(o.p.refP)
	{
		refP->refcnt::refadd(1);
	}

	//! Move constructor

	inline ptrImpl(//! An existing reference.
		       ptrImpl<objClass> &&o )
		noexcept __attribute__((artificial)) : refP(nullptr)
	{
		__atomic_exchange(&refP, &o.refP, &o.refP, __ATOMIC_SEQ_CST);
	}

	//! The default assignment operator

	ptrImpl<objClass> inline
	&operator=(//! An existing reference.

		   //! If this reference is bound to an instance of \c objClass,
		   //! its reference count gets incremented.

		   const ptrImpl<objClass> &o)
		noexcept __attribute__((artificial))
	{
		setRef_noconscheck(o.refP);

		return *this;
	}

	//! The move operator

	ptrImpl<objClass> inline
	&operator=(//! An existing reference.

		   ptrImpl<objClass> &&o)
		noexcept __attribute__((artificial))
	{
		__atomic_exchange(&refP, &o.refP, &o.refP, __ATOMIC_SEQ_CST);

		return *this;
	}

protected:

	//! Update reference pointer

	//! \internal
	//! Set the reference to point to a new instance of the object class.
	//! The new instance's reference count gets incremented. The current
	//! reference's count gets decremented. If it becomes 0 the current
	//! object the reference points to gets destroyed. The reference
	//! gets repointed to the new object.
	inline void setRef(//! New object, may be \c NULL
		    objClass *newRefP) noexcept __attribute__((artificial));

	//! Update reference pointer

	//! \internal
	//!
	//! Same as setRef(), except that newRefP is from another ptr or
	//! ref, and not a raw pointer, so we don't need to do the
	//! ref_in_constructor() check.

	inline void setRef_noconscheck(objClass *newRefP)
		noexcept __attribute__((artificial));

};

#if 0
{
#endif
}
#endif
