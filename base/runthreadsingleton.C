/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/threads/runthreadsingleton.H"
#include "x/exception.H"
#include "x/messages.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

runthreadsingleton::runthreadsingleton()
	: runthreadsingleton_mutex(mutex::create())
{
}

runthreadsingleton::~runthreadsingleton()
{
}

runthreadsingleton::get_impl::get_impl(runthreadsingleton &once_object)
	: lock(get_lock(once_object))
{
}

mutex::base::mlock 
runthreadsingleton::get_impl::get_lock(runthreadsingleton &once_object)
{
	mutex::base::mlockptr lock=
		once_object.runthreadsingleton_mutex->trylock();

	if (lock.null())
		throw EXCEPTION(libmsg(_txt("This thread is already running")));
	return lock;
}

runthreadsingleton::get_impl::~get_impl()
{
}

#if 0
{
#endif
}


