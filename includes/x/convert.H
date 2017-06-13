/*
** Copyright 2016 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_convert_H
#define x_convert_H

#include <x/namespace.h>
#include <type_traits>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Implement static or dynamic pointer conversion.

//! The default implementation uses dynamic_cast.
//!
//! \see convert

template<typename obj_from, typename obj_to, typename=void>
class convert_default_impl {

public:

	static inline obj_to *cast(obj_from *p) LIBCXX_INLINE
	{
		return dynamic_cast<obj_to *>(p);
	}

	static inline const obj_to *cast(const obj_from *p) LIBCXX_INLINE
	{
		return dynamic_cast<const obj_to *>(p);
	}
};

//! Implement static or dynamic pointer conversion.

//! Specialization for using a static cast.
//!
//! \see convert

template<typename obj_from, typename obj_to>
class convert_default_impl<obj_from, obj_to,
			   std::void_t<decltype(*(std::declval<obj_to **>())=
						(std::declval<obj_from *>()))>>{

 public:
	static inline obj_to *cast(obj_from *p) LIBCXX_INLINE
	{
		return p;
	}

	static inline const obj_to *cast(const obj_from *p) LIBCXX_INLINE
	{
		return p;
	}
};

//! Custom type pointer conversion.

//! The default implementation uses a static or a dynamic cast.

template<typename obj_from, typename obj_to, bool is_const, typename=void>
class convert_impl : public convert_default_impl<obj_from, obj_to> {};

//! Custom type pointer conversion.

//! Specialization for using cast_from().

template<typename obj_from, typename obj_to>
class convert_impl<obj_from, obj_to, false,
		   std::void_t<decltype(obj_to::cast_from
					(std::declval<obj_from *>()))>> {
public:

	static inline obj_to *cast(obj_from *p) LIBCXX_INLINE
	{
		return obj_to::cast_from(p);
	}
};

//! Custom type pointer conversion.

//! Specialization for using cast_from().

template<typename obj_from, typename obj_to>
class convert_impl<obj_from, obj_to, true,
		   std::void_t<decltype(obj_to::cast_from
					(std::declval<const obj_from *>()))>> {
public:

	static inline const obj_to *cast(const obj_from *p) LIBCXX_INLINE
	{
		return obj_to::cast_from(p);
	}
};

//! Use SFINAE for static or dynamic casting of pointers.

//! \code
//! A *foo(B *b)
//! {
//!    return convert<B,A,false>::cast(b);
//! }
//! \endcode
//!
//! If B can be static-casted to A, use that. Otherwise use dynamic_cast.
//!
//! The second parameter specifies if the pointer is const or not.
//!
//! \c A can specify a cast_from() member function:
//!
//! \code
//! class A {
//!
//! public:
//!
//!    static A *cast_from(B *);
//!    static const A *cast_from(const B *);
//! };
//! \endcode

template<typename obj_from, typename obj_to, bool is_const>
class convert : public convert_impl<obj_from, obj_to, is_const> {};

#if 0
{
#endif
}
#endif
