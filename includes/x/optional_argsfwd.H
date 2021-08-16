/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_optional_argsfwd_H
#define x_optional_argsfwd_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Parse optional arguments.

//! optional_args creates a class with a variadic list of std::optional
//! values. Each type in the variadic list results in an instance of a
//! std::optional<type>.
//!
//! The constructor's variadic parameters are used to construct each
//! std::optional on a first come, first serve basis. The default constructor
//! results in all optionals being nullopt. The first parameter to the
//! constructor initializes the first optional variadic type, provided that
//! it's constructible by it, otherwise the first parameter is used to
//! initialize the second optional, if it's constructible, and so on.
//! The first parameter gets used to construct whichever first type is
//! constructible by it. The second parameter then finds the next type that's
//! constructible by it, and so on.
//!
//! The \c LIBCXX_NAMESPACE::optional_arg works like \c std::get, to retrieve
//! the std::optional.

template<typename ...Args> class optional_args;

//! Implement \ref optional_args "optional_args" using reference_wrapper

//! This is a wrapper that takes its template parameters and implements
//! an \ref optional_args "optional_args" wrapper that uses
//! std::reference_wrapper to avoid making copies of parameter values, in
//! cases where copy elision fails, otherwise.
//!
//! The parameters to the constructor are coerced to const references, and,
//! as such, cannot be implicitly converted to any of the underlying
//! argument types. The parameters must (optionally) match the template
//! parameters, in order.

template<typename ...Args>
class optional_argconstrefs;

#if 0
{
#endif
}

#endif
