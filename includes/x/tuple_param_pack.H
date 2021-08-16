/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tuple_param_pack_H
#define x_tuple_param_pack_H

#include <x/namespace.h>
#include <string>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Helper class for converting a tuple into a parameter pack.

//! \internal
//! \c INSERT_LIBX_NAMESPACE::tuple_2_param_pack<N>::type defines
//! \c INSERT_LIBX_NAMESPACE::tuple_param_pack<0, 1, 2, ... N-1>.
//!
//! The parameters of this template class are then used as a parameter pack
//! and passed to tuple.get() using the ... operator.

template<size_t ...> class tuple_param_pack {};

//! Helper class for unpacking a tuple into a parameter pack.

//! \internal
//! This recursive template builds a parameter pack of 0...N-1, and
//! defines the \c type member as
//! \c INSERT_LIBX_NAMESPACE::tuple_param_pack<0..N-1>, where N is the
//! initial parameter to the template.
//!
//! To use this template to make a parameter pack from a std::tuple<Args...>:
//!
//! - Define a template parameter that's an opaque class.
//!
//! - Invoke the template passing
//! \c INSERT_LIBX_NAMESPACE::tuple_2_param_pack<sizeof...(Args)>::%type for
//! the opaque class template parameter.
//!
//! - Specialize your template for a "size_t ...n", with this template
//! parameter specialized as a
//! \c INSERT_LIBX_NAMESPACE::tuple_param_pack<n...>.
//!
//! - std::get<n>(tuple)... converts your tuple to a parameter pack.

template<size_t param_num, size_t ...param_num_list>
class tuple_2_param_pack
#ifndef DOXYGEN
	: public tuple_2_param_pack<param_num-1, param_num-1, param_num_list...>
#endif
{
};

//! Helper class for unpacking a tuple into a parameter pack.

//! \internal
//! The specialization stops the recursion at 0, and defines the \c type member.

template<size_t ...param_num_list>
class tuple_2_param_pack<0, param_num_list...> {
public:

	//! Helper class used to get() tuple contents, using a parameter pack notation.
	typedef tuple_param_pack<param_num_list...> type;
};

#if 0
{
#endif
}
#endif
