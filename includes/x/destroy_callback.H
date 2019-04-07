/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_destroy_callback_H
#define x_destroy_callback_H

#include <x/destroy_callbackfwd.H>
#include <x/destroy_callbackobj.H>
#include <list>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Refer to this class as \c destroy_callback::base

class destroy_callbackBase : public ptrref_base {

public:

	class guard;

	template<typename T>
	class guard_object;
};


//! Make sure that an object gets destroyed, when the guard goes out of scope.

//! Refer to this class as \c destroy_callback::base::guard

class destroy_callbackBase::guard {

	//! The owned callbacks

	std::list<destroy_callback> callbacks;

public:
	//! Deleted copy constructor
	guard(const guard &);

	//! Deleted assignment operator
	guard &operator=(const guard &)=delete;

	//! Constructor
	guard();

	//! Destructor
	~guard();

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

//! A convenient macro to instantiate a guard on a refobj.

//! Refer to this template as
//! \c INSERT_LIBX_NAMESPACE::destroy_callback::base::guard<INSERT_LIBX_NAMESPACE::ref<class,[base]>>.
//!
//! Declare this template as a class member, or instantiate it on the stack.
//! The template parameter must be a \ref ref "INSERT_LIBX_NAMESPACE::ref".
//! This template instantaties a subclass of this
//! \c INSERT_LIBX_NAMESPACE::ref with a guard object tucked away inside it.
//! When an instance of the class containing this template as a member
//! gets destroyed, or when the scope with this template instance on the
//! stack exits, execution will stop until any other references on the
//! guarded \c INSERT_LIBX_NAMESPACE::ref go out of scope, and the
//! underlying object is truly gone for good.

template<typename objClass, typename baseClass>
class destroy_callbackBase::guard_object<ref<objClass, baseClass>>
	: private guard, public ref<objClass, baseClass> {

 public:
	//! Constructor forwards whatever it gets to ref's constructor

	template<typename ...Args>
		guard_object<ref<objClass, baseClass>>(Args && ...args)
		: ref<objClass, baseClass>(std::forward<Args>(args)...)
	{
		operator()(*this);
	}

	using ref<objClass, baseClass>::operator*;
	using ref<objClass, baseClass>::operator->;
};

#if 0
{
#endif
}

#endif
