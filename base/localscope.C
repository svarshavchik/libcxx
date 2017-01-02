/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/threads/run.H"
#include "x/threads/singleton.H"
#include "x/exception.H"
#include "x/messages.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

__thread run_async::localscope *run_async::localscope::localscopeptr=0;


static std::mutex mainscopemutex;
static bool destructed=false;

run_async::localscope *run_async::localscope::mainscope=nullptr;

class LIBCXX_HIDDEN run_async::localscope::mainscope_destructor {

 public:
	mainscope_destructor() {}

	~mainscope_destructor()
	{
		localscope *p=({
				std::unique_lock<std::mutex>
					lock(mainscopemutex);
				destructed=false;

				localscope *tp=mainscope;
				mainscope=nullptr;
				destructed=true;
				tp;
			});
		if (p)
			delete p;
	}

	static mainscope_destructor instance;
};

run_async::localscope::mainscope_destructor 
run_async::localscope::mainscope_destructor::instance;

void run_async::localscope::mainscope_pushstrongobjects(const ref<obj> &p)
{
	std::unique_lock<std::mutex> lock(mainscopemutex);

	if (destructed)
		return;

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
