/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_valueimpl_H
#define x_valueimpl_H

#include <x/value_string.H>
#include <x/namespace.h>

#include <mutex>

namespace LIBCXX_NAMESPACE::property {
#if 0
};
#endif

template<typename argType, int intsizeType, bool intType> class valueImplBase;

//! valueimpl implementation for natural integer types

//! This is a specialization for natural integer types, that
//! implements get() and set() using atomic instructions.

template<typename argType, int intsizeType>
class valueImplBase<argType, intsizeType, true> {

	//! Stored %value

	mutable argType v;

public:
	//! Default constructor

	valueImplBase(argType initial_value) noexcept : v(initial_value) {}

	//! Default destructor
	~valueImplBase() {}

	//! Retrieve current %value
	argType get() const noexcept { return __sync_fetch_and_add(&v, 0); }

	//! Store a new %value

	void set(//! New %value

		      argType n) noexcept
	{
		__sync_bool_compare_and_swap(&v, __sync_fetch_and_add(&v, 0),
					     n);
	}
};

//! Specialization for a bool.

template<int intsizeType>
class valueImplBase<bool, intsizeType, true> {

	//! Stored %value

	mutable int v;

public:
	//! Default constructor

	valueImplBase(bool initial_value) noexcept : v(initial_value) {}

	//! Default destructor
	~valueImplBase() {}

	//! Retrieve current %value
	bool get() const noexcept { return __sync_fetch_and_add(&v, 0); }

	//! Store a new %value

	void set(//! New %value

		      bool n) noexcept
	{
		__sync_bool_compare_and_swap(&v, __sync_fetch_and_add(&v, 0),
					     n);
	}
};

//! valueimpl implementation for a non-int type.

//! This is a specialization for non-int types that use
//! a mutex to implement thread-safe get() and set().

template<typename argType, int intsizeType>
class valueImplBase<argType, intsizeType, false> {

	//! Stored %value
	argType v;

	//! Mutex that protects the %value
	mutable std::mutex mutex;

public:
	//! Default constructor
	valueImplBase(const argType &initial_value) noexcept : v(initial_value) {}

	//! Default destructor
	~valueImplBase() {}

	//! Retrieve current %value

	argType get() const noexcept
	{
		std::lock_guard<std::mutex> lock(mutex);

		return v;
	}

	//! Store a new %value

	void set(//! New value

		      const argType &n) noexcept
	{
		std::lock_guard<std::mutex> lock(mutex);

		v=n;
	}
};

#include <x/sys/valueimpl64sys.h>

//! A stored %property %value implementation

//! This template class defines a %property %value.
//!
//! Template parameters:
//!
//! - The %property %value's type: int, float, \c std::string
//!
//! - A boolean %value. If \c true, the %property is a natural int type that
//! can be accessed atomically. If \c false, the %property is either a floating
//! type or a class type, such as \c std::string, and a mutex protects access
//! to the value.
//!
//! The template class defines two methods:
//!
//! - get() - retrieve the %value
//!
//! - set() - store the %value
//!
//! These functions are thread safe. They use either atomic instructions, or
//! use a protecting mutex.

template<typename argType> class valueimpl
	: public valueImplBase<argType,
			       sizeof(argType),
			       std::numeric_limits<argType>::is_integer> {

	//! UNDEFINED

	//! This class does not implement a copy constructor.

	valueimpl(const valueimpl<argType> &);

	//! UNDEFINED

	//! This class does not implement an assignment operator

	valueimpl<argType &>operator=(const valueimpl<argType> &);

public:
	//! Default constructor
	valueimpl(const argType &initial_value=
		  value_string<argType>::initial_value()) noexcept
		: valueImplBase<argType, sizeof(argType),
				std::numeric_limits<argType>::is_integer
			    > (initial_value) {}

	//! Default destructor
	~valueimpl() {}

};

#if 0
{
#endif
}
#endif
