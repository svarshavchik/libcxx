/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_errors_H
#define x_errors_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Throw an "invalid cast" %exception

//! \internal
//!
void invalid_cast(const char *fromType,
		  const char *toType)
	__attribute__((noreturn));

//! Throw a "null pointer" %exception

//! \internal
//!
void null_ptr_deref() __attribute__((noreturn));

#if 0
{
#endif
}

#endif
