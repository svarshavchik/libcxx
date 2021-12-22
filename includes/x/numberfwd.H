/*
** Copyright 2016-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_numberfwd_H
#define x_numberfwd_H

#include <x/namespace.h>
#include <type_traits>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! \internal

class number_default_base;

template<typename value_type_t, typename tag,
	 typename base_class=number_default_base> class number;

template<typename> struct is_number : std::false_type {};

template<typename value_type_t, typename tag, typename base_class>
struct is_number<number<value_type_t, tag, base_class>> : std::true_type {};

template<typename T>
concept typesafe_number=is_number<T>::value;

#if 0
{
#endif
}
#endif
