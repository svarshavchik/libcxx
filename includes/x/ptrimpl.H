/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ptrimpl_H
#define x_ptrimpl_H

#include <x/concepts.H>
#include <typeinfo>
#include <utility>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Reference pointer implementation

//! This class stores the pointer to the underlying reference-counted object,
//! and implements its housekeeping functions.

template<typename objClass> class ptrImpl {

protected:
	//! Object that this reference is bound to

	//! \internal
	//! This pointer is \c NULL when this reference is not bound to
	//! any object.
	objClass *refP;

	//! Return the underlying pointer

	objClass *getrefP() const noexcept LIBCXX_INLINE
	{
		return refP;
	}
public:
	template<typename, typename> friend class const_ref;
	template<typename, typename> friend class ref;
	template<typename, typename> friend class const_ptr;
	template<typename, typename> friend class ptr;

	//! Check if this is an unbound reference

	//! \return \c true, if this reference is not bound to an instance,
	//! \c false otherwise.

	inline bool null() const noexcept LIBCXX_INLINE
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
		objClass *pArg) noexcept
		requires derived_from_obj<objClass> LIBCXX_INLINE;

	//! Default destructor
	~ptrImpl() LIBCXX_INLINE;

	//! The default copy constructor

	ptrImpl(//! An existing reference.

		//! If this reference is bound to an instance of
		//! \c objClass, its reference count gets incremented.
		const ptrImpl<objClass> &o ) noexcept
		requires derived_from_obj<objClass> LIBCXX_INLINE;

	template<typename baseClass>
	explicit
	ptrImpl(const const_ref<objClass, baseClass> &o) noexcept
		requires derived_from_obj<objClass>;

	template<typename baseClass>
	explicit
	ptrImpl(const ref<objClass, baseClass> &o) noexcept
		requires derived_from_obj<objClass>;

	template<typename baseClass>
	explicit
	ptrImpl(const_ref<objClass, baseClass> &&o) noexcept
		requires derived_from_obj<objClass>;

	template<typename baseClass>
	inline explicit
	ptrImpl(ref<objClass, baseClass> &&o) noexcept
		requires derived_from_obj<objClass>;

	//! Move constructor

	inline ptrImpl(//! An existing reference.
		       ptrImpl<objClass> &&o )
		noexcept
		requires derived_from_obj<objClass> LIBCXX_INLINE
		: refP{o.refP}
	{
		o.refP=nullptr;
	}

	//! Move constructor

	template<typename baseClass>
	inline ptrImpl(//! An existing reference.
		       const_ptr<objClass, baseClass> &&o )
		noexcept
		requires derived_from_obj<objClass>;

	//! Move constructor

	template<typename baseClass>
	inline ptrImpl(//! An existing reference.
		       ptr<objClass, baseClass> &&o )
		noexcept
		requires derived_from_obj<objClass>;

	//! The default assignment operator

	ptrImpl<objClass> inline
	&operator=(//! An existing reference.

		   //! If this reference is bound to an instance of \c objClass,
		   //! its reference count gets incremented.

		   const ptrImpl<objClass> &o)
		noexcept LIBCXX_INLINE
	{
		setRef_noconscheck(o.refP);

		return *this;
	}

	//! The move operator

	ptrImpl<objClass> inline
	&operator=(//! An existing reference.

		   ptrImpl<objClass> &&o)
		noexcept LIBCXX_INLINE
	{
		std::swap(refP, o.refP);

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
		    objClass *newRefP) noexcept LIBCXX_INLINE;

	//! Update reference pointer

	//! \internal
	//!
	//! Same as setRef(), except that newRefP is from another ptr or
	//! ref, and not a raw pointer, so we don't need to do the
	//! ref_in_constructor() check.

	inline void setRef_noconscheck(objClass *newRefP)
		noexcept LIBCXX_INLINE;

};

	//! Move constructor
template<typename objClass>
template<typename baseClass>
inline ptrImpl<objClass>::ptrImpl(//! An existing reference.
	       const_ptr<objClass, baseClass> &&o )
	noexcept requires derived_from_obj<objClass> : refP{nullptr}
{
	std::swap(refP, o.refP);
}

	//! Move constructor
template<typename objClass>
template<typename baseClass>
inline ptrImpl<objClass>::ptrImpl(//! An existing reference.
				  ptr<objClass, baseClass> &&o )
	noexcept requires derived_from_obj<objClass> : refP{nullptr}
{
	std::swap(refP, o.refP);
}

#define x_ptrimpl_H_included

#include <x/ptrimpl_obj.H>

#if 0
{
#endif
}

#endif
