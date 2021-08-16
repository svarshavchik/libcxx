/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_sessioncache_H
#define x_tls_sessioncache_H

#include <x/namespace.h>
#include <x/gnutls/sessioncachefwd.H>
#include <x/gnutls/sessioncacheobj.H>
#include <x/ref.H>

namespace LIBCXX_NAMESPACE::gnutls {
#if 0
}
#endif

//! Base class for a session cache reference pointer.

//! This is the base class for \ref sessioncache "INSERT_LIBX_NAMESPACE::gnutls::sessioncache"
//! and  \ref sessioncacheptr "INSERT_LIBX_NAMESPACE::gnutls::sessioncache"

class sessioncacheBase {

public:

	//! Create a basic implementation that uses an \ref ordered_cache "simple ordered cache".
	static sessioncache create();

	//! Default factory that creates a basic implementation that uses an ordered cache.

	template<typename ptrrefType> class objfactory
	{
	public:


		//! The default %create() method.

		template<typename... Args_t>
		static inline ptrrefType
		create(Args_t &&...args)
		{
			return sessioncacheBase
				::create(std::forward<Args_t>(args)...);
		}
	};

};

#if 0
{
#endif
}
#endif
