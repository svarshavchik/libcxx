/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fdtimeout.H"
#include "x/fdtimeoutconfig.H"
#include "x/fd.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

fdbase fdtimeoutconfig::operator()(const fd &fdArg) const
{
	return fdArg;
}

fdtimeoutconfig::terminate_fd::terminate_fd(const fd &terminate_fdArg)
 : fdref(terminate_fdArg)
{
}

fdtimeoutconfig::terminate_fd::~terminate_fd()
{
}

fdbase fdtimeoutconfig::terminate_fd::operator()(const fd &fdArg)
	const
{
	fdtimeout t(fdtimeout::create(fdArg));

	t->set_terminate_fd(fdref);

	return t;
}

#if 0
{
#endif
}
