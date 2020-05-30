/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_serializationfwd_H
#define x_serializationfwd_H

#include <x/container_traits.H>
#include <type_traits>

namespace LIBCXX_NAMESPACE::serialization {
#if 0
};
#endif

template<typename int_type> class serialize_integer_type;

//! Default serialization traits

//! The serialization traits class defines a single static function:
//! classcreate(), which initializes a reference to an object just before
//! the referenced object gets deserialized.

class default_traits {

public:
	//! Instantiate a reference to a deserialized class

	template<typename ref_type>
	static void classcreate(ref_type &ptr)
	{
		if (ptr.null())
			ptr=ref_type::create();
	}
};

//! A datatype that can be serialized as an integer

//! An integer type, with serialize_integer_type mapping known enums to
//! integer values.
template<typename T>
concept serializable_integer=std::is_integral_v<
	typename serialize_integer_type<T>::int_t>;

//! A serializable floating point datatype.

template<typename T>
concept serializable_float=std::is_floating_point<T>::value;

//! A serializable container.

template<typename T>
concept serializable_container=container_traits<T>::type != unknown_container;

#if 0
{
#endif
}
#endif
