/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ptr.H"
#include "x/property_value.H"
#include "x/exception.H"

#include <cstring>
#include <cstdlib>
#include <cxxabi.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

static property::value<bool> invalid_cast_exception(LIBCXX_NAMESPACE_STR
						    "::exception::abort::cast",
						    false);

static property::value<bool> null_ptr_exception(LIBCXX_NAMESPACE_STR
						"::exception::abort::null",
						false);

void invalid_cast(const char *fromTypeArg,
		  const char *toTypeArg)
{
	int status;

	char *fromType=abi::__cxa_demangle(fromTypeArg, 0, 0, &status);

	if (status)
		fromType=0;

	char *toType=abi::__cxa_demangle(toTypeArg, 0, 0, &status);

	if (status)
		toType=0;

	char fmtbuf[strlen(fromType)+strlen(toType)+100];

	strcat(strcat(strcat(strcpy(fmtbuf, "Object cannot be casted from "),
			     fromType ? fromType:fromTypeArg), " to "),
	       toType ? toType:toTypeArg);

	if (fromType)
		free(fromType);

	if (toType)
		free(toType);

	if (invalid_cast_exception.get())
	{
		std::cerr << fmtbuf << std::endl << std::flush;
		abort();
	}

	throw EXCEPTION(&fmtbuf[0]);
}

void null_ptr_deref()
{
	static const char msg[]="Null pointer dereference";

	if (null_ptr_exception.get())
	{
		std::cerr << msg << std::endl << std::flush;
		abort();
	}

	throw EXCEPTION(msg);
}

#if 0
{
#endif
}
