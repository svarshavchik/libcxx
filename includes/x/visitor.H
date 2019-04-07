/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_visitor_H
#define x_visitor_H

#include <utility>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A visitor for a \c std::variant
//!
//! \code
//! struct one{};
//! struct two{};
//!
//! void foo(const std::variant<one, two> &bar)
//! {
//! 	std::visit( INSERT_LIBX_NAMESPACE::visitor{
//! 			[](const one &o) {},
//! 			[](const two &t) {}
//! 		}, bar);
//! }
//! \endcode
//!
//! Template constructs a callable object for passing to a
//! \c std::visit, for a particular \c std::variant.

template<typename ...Functors> struct visitor : Functors...
{
	using Functors::operator()...;
};

//! Deduction guide for a \ref visitor "visitor".

template<class... Functors> visitor(Functors...) -> visitor<Functors...>;

#if 0
{
#endif
}
#endif
