/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifdef x_ptrimpl_H_included
#ifdef x_obj_H_included

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
