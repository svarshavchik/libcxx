/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_refptr_hash_H
#define x_refptr_hash_H

#include <functional>
#include <x/ptrfwd.H>

namespace std {

	//! Specialize \c std::hash for \c INSERT_LIBX_NAMESPACE::ref

	template<typename objClass, typename baseType>
	struct hash<LIBCXX_NAMESPACE::ref<objClass, baseType>> {

		inline size_t operator()(const LIBCXX_NAMESPACE::ref<objClass,
					 baseType> &r) const noexcept
		{
			return (size_t)(&*r);
		}
	};


	//! Specialize \c std::hash for \c INSERT_LIBX_NAMESPACE::const_ref

	template<typename objClass, typename baseType>
	struct hash<LIBCXX_NAMESPACE::const_ref<objClass, baseType>> {

		inline size_t operator()(const LIBCXX_NAMESPACE::
					 const_ref<objClass,
					 baseType> &r) const noexcept
		{
			return (size_t)(&*r);
		}
	};

	//! Specialize \c std::hash for \c INSERT_LIBX_NAMESPACE::ptr

	template<typename objClass, typename baseType>
	struct hash<LIBCXX_NAMESPACE::ptr<objClass, baseType>> {

		inline size_t operator()(const LIBCXX_NAMESPACE::ptr<objClass,
					 baseType> &r) const noexcept
		{
			return (size_t)(&*r);
		}
	};


	//! Specialize \c std::hash for \c INSERT_LIBX_NAMESPACE::const_ptr

	template<typename objClass, typename baseType>
	struct hash<LIBCXX_NAMESPACE::const_ptr<objClass, baseType>> {

		inline size_t operator()(const LIBCXX_NAMESPACE::
					 const_ptr<objClass,
					 baseType> &r) const noexcept
		{
			return (size_t)(&*r);
		}
	};
};


#endif
