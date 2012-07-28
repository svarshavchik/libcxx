/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "stopexception.H"
#include "messages.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

stopexception::stopexception() noexcept
{
	(*this) << libmsg(_txt("Thread stopped"));

	(*this)->save();
}

stopexception::~stopexception() noexcept
{
}

#if 0
{
#endif
}
