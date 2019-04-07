/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mp_H
#define x_mp_H

#include <x/mpobj.H>
#include <x/obj.H>
#include <x/ref.H>

#include <utility>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Helper object used by \ref mp "INSERT_LIBX_NAMESPACE::mp".

template<typename obj_type>
class mp_baseObj : virtual public obj, public obj_type {

public:

	//! Constructor

	template<typename ...Args>
	mp_baseObj(Args && ...args)
		: obj_type{std::forward<Args>(args)...}
	{
	}

};

//! Mutex-protected object.

//! A reference-counted wrapper based on
//! \ref mpobj "INSERT_LIBX_NAMESPACE::mpobj".
//!
//! The template parameter is a class that's copy-constructible.
//!
//! Arguments to the constructor get forwarded to the class.
//!
//! get() returns an \ref const_ref "constant reference pointer" to the
//! current object. The object is locked only for the duration of the
//! get() call, and the referenced object is constant, and does not get
//! affected by any furture update()s. The -> operator is an alias for get().
//!
//! update()'s parameter is a functor and it receives a reference to the
//! wrapped object instance, which it may modify. The wrapper object is locked
//! for the duration of the update().
//!
//! A single thread may make recursive update()s, but each update() functor
//! gets a reference to a different object, and only the initial update()'s
//! results remain after it is unlocked. An exception thrown inside an update()
//! results in the original object remaining in place, only a successfully
//! completed update() results in a modified object.

template<typename obj_type>
class mp {

	//! Internal container.

	typedef mpobj<ref<mp_baseObj<obj_type>>,
		      std::recursive_mutex> current_value_t;

	//! The current value of the object.
	mutable current_value_t current_value;

public:

	//! Constructor
	template<typename ...Args>
	mp(Args && ...args)
		: current_value{ref<mp_baseObj<obj_type>>::create
			(std::forward<Args>(args)...)}
	{
	}

	//! Return the current object.

	inline const_ref<mp_baseObj<obj_type>> get() const
	{
		typename current_value_t::lock lock{current_value};

		return *lock;
	}

	//! Overloaded operator.
	inline auto operator->() const
	{
		return get();
	}

	//! Update the object.

	template<typename functor>
	inline void update(functor &&f)
	{
		typename current_value_t::lock lock{current_value};

		auto new_value=
			ref<mp_baseObj<obj_type>>::create
			( static_cast<const obj_type &>(**lock));

		f(static_cast<obj_type &>(*new_value));

		*lock=new_value;
	}
};

#if 0
{
#endif
}
#endif
