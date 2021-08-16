/*
** Copyright 2020-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_explicit_arg_H
#define x_explicit_arg_H

#include <x/namespace.h>
#include <x/ptrfwd.H>
#include <type_traits>

namespace LIBCXX_NAMESPACE {
#if 0
}
#endif

//! No overload resolution for a function parameter.

//! \code
//! void function(const INSERT_LIBX_NAMESPACE::explicit_arg<someclass> &arg);
//! \endcode
//!
//!
//! Accept only a given ref or a ptr, as a parameter.

//! \see explicit_refptr

template<typename T>
struct explicit_arg : T {

	//! Constructor
	template<typename Arg,
		 typename=std::enable_if_t<
			 std::is_same_v<std::remove_cvref_t<Arg>, T>>>
	explicit_arg(Arg &&arg)
		: T{std::forward<Arg>(arg)}
	{
	}

	using T::operator<=>;
	using T::operator==;

	//! Inherited ! operator.
	inline bool operator!() const noexcept
	{
		return T::operator!();
	}

	//! Inherited bool operator
	inline operator bool() const noexcept
	{
		return T::operator bool();
	}
};

#if 0
{
#endif
}
#endif
