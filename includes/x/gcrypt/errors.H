/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_gcrypt_errors_H
#define x_gcrypt_errors_H

#include <x/namespace.h>
#include <gcrypt.h>

namespace LIBCXX_NAMESPACE {

	//! Algorithms

	//! This namespace exports useful interfaces from the underlying
	//! libgcrypt library. Link with -lcxxtls to access these methods.

	namespace gcrypt {
#if 0
	}
}
#endif

//! Throw an exception after an error.

//! \internal

void chkerr_throw(gcry_error_t code);

//! Check the return value from a gcrypt library function call.

//! \internal

inline void chkerr(gcry_error_t code)
{
	if (code)
		chkerr_throw(code);
}

#if 0
{
	{
#endif
	}
}
#endif
