/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_visitor_H
#define x_visitor_H

#include <utility>
#include <type_traits>
#include <variant>

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

/*! Determine if a type is included in a variant.

\code
typedef std::variant<int, bool> v;

template<typename T, typename U=std::enable_if_t<INSERT_LIBX_NAMESPACE::visited<T, v>::value>>
struct is_in_v{};

is_in_v<int> this_compiles;

is_in_v<const char *> compilation_error;
\endcode

Determines whether T is one of the values in T, which must be a std::variant.

 */

template<typename, typename >
struct visited;

/*! Implementation of \ref visited "INSERT_LIBX_NAMESPACE::visited" */

template<typename T, typename ...variant_types>
struct visited<T, std::variant<variant_types...>>
  : public std::disjunction<std::is_same<T, variant_types>...> {};

/*! Determine if a type is included in a variant.

\see visited

*/

template<typename T, typename V>
constexpr bool visited_v=visited<T, V>::value;

#if 0
{
#endif
}
#endif
