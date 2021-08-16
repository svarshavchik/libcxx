/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/stopexception.H"
#include "x/messages.H"
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

stopexception::~stopexception()
{
}

#if 0
{
#endif
}
