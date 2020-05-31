/*
** Copyright 2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_concepts_H
#define x_concepts_H

#include <x/namespace.h>
#include <type_traits>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class LIBCXX_PUBLIC obj;
template<typename, typename> class const_ref;
template<typename, typename> class ref;
template<typename, typename> class const_ptr;
template<typename, typename> class ptr;

//! Objects are derived from \ref obj "obj"

template<typename T>
concept is_derived_from_obj = std::is_base_of<obj, T>::value;

//! Helper class for the is_derived_from_ref_t concept

template<typename T>
struct is_derived_from_ref_or_ptr_v
{
	static std::false_type test(...);

	template <typename objClass, typename baseClass>
	static std::true_type test(const const_ptr<objClass, baseClass> &);

	template <typename objClass, typename baseClass>
	static std::true_type test(const ptr<objClass, baseClass> &);

	template <typename objClass, typename baseClass>
	static std::true_type test(const const_ref<objClass, baseClass> &);

	template <typename objClass, typename baseClass>
	static std::true_type test(const ref<objClass, baseClass> &);

	static constexpr bool const value =
		decltype(test(std::declval<const T &>()))::value;
};

//! The given type is a ref, ptr, const_ref, const_ptr, or is derived from them

template<typename T>
concept is_derived_from_ref_or_ptr = is_derived_from_ref_or_ptr_v<T>::value;

//! Determine what type to use to compare two references or pointers.

//! Defines the type_t member as follows

template<typename objClass, typename objClass2> struct common_obj {

	//! The default type_t is \ref obj "INSERT_LIBX_NAMESPACE::obj"

	//! All objects get derived from it.
	typedef obj type_t;
};

//! Specialization when comparing two references/pointers to the same class

//! Makes it possible to compare them even if the referenced class is an
//! undefined forward reference.
template<typename objClass>
struct common_obj<objClass, objClass> {
	typedef objClass type_t;
};

//! Which class to use to compare two pointers/references to.

template<typename objClass, typename objClass2>
using common_obj_t=common_obj<objClass, objClass2>::type_t;

#if 0
{
#endif
}
#endif
