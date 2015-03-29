/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/interval.H"

#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

void interval_base::invalid_constructor()
{
	throw EXCEPTION(_("Invalid parameters to the interval constructor class"));
}

void interval_base::syntax_error()
{
	throw EXCEPTION(_("Cannot parse specified interval"));
}

#if 0
{
#endif
}
