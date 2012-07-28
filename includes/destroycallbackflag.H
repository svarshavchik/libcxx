/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_destroycallbackflag_H
#define x_destroycallbackflag_H

#include <x/destroycallbackflagfwd.H>
#include <x/destroycallbackflagobj.H>

#include <list>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Refer to this class as \c destroyCallbackFlag::base

class destroyCallbackFlagBase : public ptrrefBase {

public:

	class guard;
};


//! Make sure that an object gets destroyed, when the guard goes out of scope.

//! Refer to this class as \c destroyCallbackFlag::base::guard

class destroyCallbackFlagBase::guard {

	//! The owned callbacks

	std::list<destroyCallbackFlag> callbacks;

public:
	//! Constructor
	guard();

	//! Destructor
	~guard() noexcept;

	//! This object must be destroyed before the guard goes out of scope
	template<typename objClass, typename baseClass>
	inline ref<objClass, baseClass>
	operator()(const ref<objClass, baseClass> &refArg)
	{
		add(refArg);
		return refArg;
	}

private:
	//! Implementation of the () operator

	//! \internal
	void add(const x::ref<x::obj> &obj);
};

#if 0
{
#endif
}

#endif

