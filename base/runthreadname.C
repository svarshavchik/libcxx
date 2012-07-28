/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/threads/runthreadname.H"
#include "x/threads/runfwd.H"
#include <sstream>
#include <thread>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

__thread const char *run_async::thread_name="main";

const char run_async::default_name[]="thread";

std::string runthreadname::get_default_name()
{
	std::ostringstream o;

	o << "thread(" << std::this_thread::get_id() << ")";

	return o.str();
}

// Initial thread name

run_async::name::name(const std::string &nArg)
	: n(nArg)
{
	thread_name=n.c_str();
}

// Thread destruction: reset name of the thread back to the default

// This thread name object on the executing thread's stack is about
// to be destroyed.

run_async::name::~name() noexcept
{
	thread_name=default_name;
}

#if 0
{
#endif
}


