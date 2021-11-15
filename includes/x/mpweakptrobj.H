/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mpweakptrobj_H
#define x_mpweakptrobj_H

#include <x/mpweakptrfwd.H>
#include <x/weakptrfwd.H>
#include <x/ptrfwd.H>
#include <x/mpobj.H>
#include <x/namespace.h>
#include <type_traits>
#include <utility>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename objClass, typename baseType>
class mpweakptrObj<ptr<objClass, baseType>> : virtual public obj {

public:
	typedef mpobj<weakptr<ptr<objClass, baseType>>> w_t;

	mutable w_t w;

public:

	//! Constructor
	template<typename ...Args,
		 typename=std::void_t<decltype(
		weakptr<ptr<objClass, baseType>>
		{
			std::declval<Args &&>()...
		})>>
	mpweakptrObj(Args && ...args)
		: w{std::forward<Args>(args)...}
	{
	}

	~mpweakptrObj()=default;

	//! Update the weakly-referenceo object.

	template<typename Arg>
	void setptr(Arg &&arg)
	{
 		typename w_t::lock lock{w};

		*lock=std::forward<Arg>(arg);
	}

	//! Retrieve the weakly-reference object.

	ptr<objClass, baseType> getptr() const
	{
 		typename w_t::lock lock{w};

		return lock->getptr();
	}
};

#if 0
{
#endif

}
#endif
