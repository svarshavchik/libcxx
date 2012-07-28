/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/rwmutexdebug.H"
#include "x/exception.H"
#include "gettext_in.h"
#include <iostream>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::rwmutexdebug);

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

void rwmutexdebug::oops()
{
	auto e=EXCEPTION(libmsg(_txt("Recursive rwmutex lock detected")));

	std::cerr << e->backtrace << std::flush;
	throw e;
}

void rwmutexdebug::debuglockbase::own()
{
	lockpool_t::lock lock(lockpool);

	if (!lock->insert(std::this_thread::get_id()).second)
		oops();
}

void rwmutexdebug::debuglockbase::unown()
{
	lockpool_t::lock lock(lockpool);

	lock->erase(std::this_thread::get_id());
}

rwmutexdebug::debuglockbase::debuglockbase(lockpool_t &lockpoolArg)
	: lockpool(lockpoolArg)
{
}

rwmutexdebug::rwmutexdebug() : r(lockpool, rwmutex::r),
			       w(lockpool, rwmutex::w)
{
}

rwmutexdebug::~rwmutexdebug() noexcept
{
}

#if 0
{
#endif
}
