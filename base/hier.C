/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/hierobj.H"
#include "x/messages.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

void cant_clone_writelock()
{
	throw EXCEPTION(libmsg(_txt("Attempt to clone a write lock on a hier container")));
}

#if 0
{
#endif
}
