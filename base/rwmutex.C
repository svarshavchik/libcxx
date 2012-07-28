/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "rwmutex.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

rwmutex::wmutex::wmutex() : nreaders(0), wlocked(false)
{
}

rwmutex::rmutex::rmutex(wmutex &wArg) : w(wArg)
{
}

rwmutex::rwmutex() : r(w)
{
}

rwmutex::~rwmutex() noexcept
{
}

void rwmutex::wmutex::lock()
{
	std::unique_lock<std::mutex> lock(mutex);

	bool captured=false;

	try {
		cond.wait(lock, [&captured, this]
			  {
				  if (!wlocked)
					  captured=wlocked=true;

				  return captured && nreaders == 0;
			  });
	} catch (...) {
		if (captured)
		{
			wlocked=false;
			cond.notify_all();
		}
		throw;
	}
}

void rwmutex::wmutex::unlock()
{
	std::lock_guard<std::mutex> lock(mutex);

	wlocked=false;
	cond.notify_all();
}

bool rwmutex::wmutex::try_lock()
{
	std::lock_guard<std::mutex> lock(mutex);

	if (wlocked || nreaders)
		return false;

	wlocked=true;
	return true;
}

void rwmutex::rmutex::lock()
{
	std::unique_lock<std::mutex> lock(w.mutex);

	w.cond.wait(lock, [this]
		    {
			    return !w.wlocked;
		    });
	++w.nreaders;
}

void rwmutex::rmutex::unlock()
{
	std::lock_guard<std::mutex> lock(w.mutex);

	--w.nreaders;
	w.cond.notify_all();
}

bool rwmutex::rmutex::try_lock()
{
	std::lock_guard<std::mutex> lock(w.mutex);

	if (w.wlocked)
		return false;

	++w.nreaders;
	return true;
}

#if 0
{
#endif
}
