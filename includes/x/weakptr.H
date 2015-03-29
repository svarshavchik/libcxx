/*
** Copyright 2012-2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakptr_H
#define x_weakptr_H

#include <x/ref.H>
#include <x/exception.H>
#include <x/weakptrfwd.H>
#include <x/weakinfo.H>
#include <x/obj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename objClass, typename baseType>
inline weakptr<ptr<objClass, baseType>>::weakptr()
{
}

template<typename objClass, typename baseType>
inline weakptr<ptr<objClass, baseType>>::weakptr(const ptrType &refArg)
{
	if (!refArg.null())
		ptr<weakinfo>::operator=(refArg->obj::weak());
}

template<typename objClass, typename baseType>
inline weakptr<ptr<objClass, baseType>>::weakptr(const ref<objClass, baseType>
						 &refArg)
{
	ptr<weakinfo>::operator=(refArg->obj::weak());
}

template<typename objClass, typename baseType>
template<typename objClass2, typename baseType2>
inline weakptr<ptr<objClass, baseType>>
::weakptr(const ptr<objClass2, baseType2> &ptrArg)
	: weakptr(ptrType(ptrArg))
{
}

template<typename objClass, typename baseType>
template<typename objClass2, typename baseType2>
inline weakptr<ptr<objClass, baseType>>
::weakptr(const ref<objClass2, baseType2> &refArg)
	: weakptr(ref<objClass, baseType>(refArg))
{
}

template<typename objClass, typename baseType>
inline weakptr<ptr<objClass, baseType>>::~weakptr() noexcept
{
}

template<typename objClass, typename baseType>
inline weakptr<ptr<objClass, baseType>> &weakptr<ptr<objClass, baseType>>
	::operator=(const ptrType &r)

{
	(*this)=weakptr<ptrType>(r);

	return *this;
}

//! Assignment operator
template<typename objClass, typename baseType>
template<typename objClass2, typename baseType2>
inline weakptr<ptr<objClass, baseType>> &weakptr<ptr<objClass, baseType>>
	::operator=(const ptr<objClass2, baseType2> &r)

{
	(*this)=weakptr<ptrType>(ptrType(r));

	return *this;
}

template<typename objClass, typename baseType>
inline weakptr<ptr<objClass, baseType>> &weakptr<ptr<objClass, baseType>>
	::operator=(const ref<objClass, baseType> &r)

{
	return *this = ptrType(r);
}

//! Assignment operator
template<typename objClass, typename baseType>
template<typename objClass2, typename baseType2>
inline weakptr<ptr<objClass, baseType>> &weakptr<ptr<objClass, baseType>>
	::operator=(const ref<objClass2, baseType2> &r)

{
	return *this = ptrType(r);
}

template<typename objClass, typename baseType>
inline ptr<objClass, baseType> weakptr<ptr<objClass, baseType>>::getptr() const
{
	if (!refP)
		return ptrType();

	return refP->getstrongref();
}

#if 0
{
#endif

}
#endif
