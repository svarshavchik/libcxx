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
#include <pthread.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

__thread run_async::localscope *run_async::localscope::localscopeptr=0;

// Objects that get destroyed at application shutdown get pushed into
// mainscope.
//
// mainscopemutex protects the 'destructed' flag and the 'mainscope' pointer.

namespace {
	struct static_instance_t {

		pthread_mutex_t instance_mutex = PTHREAD_MUTEX_INITIALIZER;
		bool destructed = false;
	};
}

static static_instance_t &get_main_instance()
{
	static static_instance_t instance;

	return instance;
}

run_async::localscope *run_async::localscope::mainscope=nullptr;

#include "localscope.H"

mainscope_destructor::mainscope_destructor()=default;

mainscope_destructor::~mainscope_destructor()
{
	auto &main_instance=get_main_instance();

	auto p=({
			pthread_mutex_lock(&main_instance.instance_mutex);
			main_instance.destructed=false;

			auto *tp=run_async::localscope::mainscope;
			run_async::localscope::mainscope=nullptr;
			main_instance.destructed=true;
			pthread_mutex_unlock(&main_instance.instance_mutex);
			tp;
		});
	if (p)
		delete p;
}

void run_async::localscope::register_singleton(const ref<obj> &p)
{
	auto &main_instance=get_main_instance();
	pthread_mutex_lock(&main_instance.instance_mutex);

	if (main_instance.destructed)
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
	pthread_mutex_unlock(&main_instance.instance_mutex);
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
