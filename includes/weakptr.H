/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_weakptr_H
#define x_weakptr_H

#include <x/ptr.H>
#include <x/exception.H>
#include <x/weakptrfwd.H>
#include <x/weakinfo.H>
#include <x/destroycallbackobj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename ptrType>
inline weakptr<ptrType>::weakptr()
{
}

template<typename ptrType>
inline weakptr<ptrType>::weakptr(const ptrType &refArg)
{
	if (!refArg.null())
		ptr<weakinfo>::operator=(refArg->obj::weak());
}

template<typename ptrType>
inline weakptr<ptrType>::~weakptr() noexcept
{
}

template<typename ptrType>
inline weakptr<ptrType> &weakptr<ptrType>::operator=(const ptrType &r)

{
	(*this)=weakptr<ptrType>(r);

	return *this;
}

template<typename ptrType>
inline ptrType weakptr<ptrType>::getptr() const
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
