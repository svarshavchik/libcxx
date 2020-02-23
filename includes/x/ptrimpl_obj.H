/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifdef x_ptrimpl_H_included
#ifdef x_obj_H_included

template<typename objClass>
inline void ptrImpl<objClass>::setRef(objClass *newRefP) noexcept
{
	if (newRefP && ++newRefP->obj::refcnt == 0)
		ref_in_constructor(newRefP);

	std::swap(refP, newRefP);

	if (newRefP && --newRefP->obj::refcnt == 0)
		newRefP->obj::destroy();
}

template<typename objClass>
inline void ptrImpl<objClass>::setRef_noconscheck(objClass *newRefP) noexcept
{
	if (newRefP)
		++newRefP->obj::refcnt;

	std::swap(refP, newRefP);

	if (newRefP && --newRefP->obj::refcnt == 0)
		newRefP->obj::destroy();
}

template<typename objClass>
inline ptrImpl<objClass>::ptrImpl(objClass *pArg)
	noexcept
	: refP(pArg)
{
	if (refP && ++refP->obj::refcnt == 0)
		ref_in_constructor(refP);
}

template<typename objClass>
inline ptrImpl<objClass>::~ptrImpl()
{
	if (refP && --refP->obj::refcnt == 0)
		refP->obj::destroy();
}

template<typename objClass>
inline ptrImpl<objClass>::ptrImpl(const ptrImpl<objClass> &o )
	noexcept : refP(o.refP)
{
	if (refP)
		++refP->obj::refcnt;
}

//! Copy constructor from a known non-NULL reference

template<typename objClass>
template<typename baseClass>
inline
ptrImpl<objClass>::ptrImpl(const const_ref<objClass, baseClass> &o) noexcept
	: refP(o.p.refP)
{
	obj &thiso=*refP;
	++thiso.refcnt;
}

//! Copy constructor from a known non-NULL reference

template<typename objClass>
template<typename baseClass>
inline
ptrImpl<objClass>::ptrImpl(const ref<objClass, baseClass> &o) noexcept
	: refP(o.p.refP)
{
	obj &thiso=*refP;
	++thiso.refcnt;
}

//! Move constructor from a known non-NULL reference

template<typename objClass>
template<typename baseClass>
inline
ptrImpl<objClass>::ptrImpl(const_ref<objClass, baseClass> &&o) noexcept
	: refP(o.p.refP)
{
	o.p.refP=nullptr;
}

//! Move constructor from a known non-NULL reference

template<typename objClass>
template<typename baseClass>
inline
ptrImpl<objClass>::ptrImpl(ref<objClass, baseClass> &&o) noexcept
	: refP(o.p.refP)
{
	o.p.refP=nullptr;
}

#endif
#endif
