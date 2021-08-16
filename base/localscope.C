/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/threads/run.H"
#include "x/threads/singleton.H"
#include "x/exception.H"
#include "x/messages.H"
#include "gettext_in.h"
#include <cstdlib>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

__thread run_async::localscope *run_async::localscope::localscopeptr=0;

// Objects that get destroyed at application shutdown get pushed into
// mainscope.
//
// mainscopemutex protects the 'destructed' flag and the 'mainscope' pointer.

static std::mutex mainscopemutex;
static bool destructed=false;

run_async::localscope *run_async::localscope::mainscope=nullptr;

#include "localscope.H"

mainscope_destructor::mainscope_destructor()=default;

mainscope_destructor::~mainscope_destructor()
{
	auto p=({
			std::unique_lock<std::mutex>
				lock{mainscopemutex};
			destructed=false;

			auto *tp=run_async::localscope::mainscope;
			run_async::localscope::mainscope=nullptr;
			destructed=true;
			tp;
		});
	if (p)
		delete p;
}

void run_async::localscope::register_singleton(const ref<obj> &p)
{
	std::unique_lock<std::mutex> lock(mainscopemutex);

	if (destructed)
	{
		static const char fatal[]=
			"Attempting to construct a singleton during "
			"the global destruction phase.\n";

		if (write(2, fatal, sizeof(fatal)-1) < 0)
			;
		abort();
	}
	if (!mainscope)
		mainscope=new localscope;

	mainscope->strongobjects.push_back(p);
}

run_async::localscope::localscope()
	: mcguffin(ref<obj>::create())
{
	localscopeptr=this;
}

run_async::localscope::~localscope()
{
	localscopeptr=0;
}

void run_async::no_local_variables()
{
	throw EXCEPTION(libmsg(_txt("Thread local variables not available")));
}

void run_async::singletonbase::morethanone()
{
	throw EXCEPTION(libmsg(_txt("Multiple declarations of the same singleton")));
}


bool run_async::singletonbase::savestrongobject(const ref<obj> &refobj)
{
	if (!localscope::localscopeptr)
		return false;

	localscope::localscopeptr->strongobjects.push_back(refobj);
	return true;
}

#if 0
{
#endif
}
