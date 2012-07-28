/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "vectorobj.H"
#include "sysexception.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

void vectorObjBase::read_error()
{
	throw SYSEXCEPTION("read");
}

void vectorObjBase::write_error()
{
	throw SYSEXCEPTION("write");
}

#if 0
{
#endif
}
