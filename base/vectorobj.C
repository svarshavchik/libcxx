/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/vectorobj.H"
#include "x/sysexception.H"

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
