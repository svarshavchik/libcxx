/*
** Copyright 2012-2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "obj.H"
#include "exception.H"
#include "namespace.h"
#include "mutex.H"
#include "weakinfo.H"
#include "ref.H"
#include "destroycallback.H"
#include "logger.H"
#include "exception.H"
#include "messages.H"

#include <cstdlib>
#include <iostream>
#include <cxxabi.h>
#include "gettext_in.h"

#ifndef SELFTEST_HOOK
#define SELFTEST_HOOK() do { } while (0)
#endif

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::obj, objlog);

void ref_in_constructor(obj *p)
{
	LOG_FUNC_SCOPE(objlog);

	exception e;

	LOG_FATAL(gettextmsg(libmsg(_txt("Attempting to create an x::ref(this) or x::ptr(this) in %1%'s constructor")), p->objname()));
	LOG_TRACE(e->backtrace);
	abort();
}

obj::obj() noexcept : refcnt(-1)
{
}

obj::~obj() noexcept
{
}


void obj::demangle(const char *mangled,
		   std::string &demangled) noexcept
{
	int status;
	char *t=abi::__cxa_demangle(mangled, 0, 0, &status);

	if (status == 0)
	{
		try {
			demangled=t;
			free(t);
		} catch (...) {
			free(t);
			throw;
		}
	}
	else
		demangled=mangled;
}

//! Name of this object

std::string obj::objname() const
{
	std::string n;

	demangle(typeid(*this).name(), n);
	return n;
}

void obj::destroy() noexcept
{
	/*
	** weakinforef gets initialized when the first weak reference gets
	** created for this object. Therefore, if weakinforef is null, and
	** we're here, then this is the last remaining reference to the object
	** and it is not possible for any other thread to have any other
	** reference, of any kind, to it.
	*/

	weakinfo *weakinfop;

	{
		std::lock_guard<std::mutex> weaklock(objmutex);

		weakinfop=weakinforef.refP;

		SELFTEST_HOOK();
	}

	std::list<ref<destroyCallbackObj> > cb_list_cpy;

	if (!weakinfop)
	{
		try {

			refadd(-1); // Prevent ref(this) in the destructor
			delete this;
		} catch (const exception &e)
		{
			std::cerr << e << std::endl
				  << e->backtrace << std::flush;
			abort();
		}
		return;
	}

	try {
		// Make sure that the weak info object doesn't go out of scope.

		ref<weakinfo> weakinforef_cpy(weakinforef);

		weakinfo &wi= *weakinfop;

		std::lock_guard<std::recursive_mutex>
			destroy_mutex_lock(wi.destroy_mutex);

		{
			std::lock_guard<std::mutex> weaklock(wi.mutex);

			if ( refget() > 0)
			{
				// Someone obtained a strong ptr on this object
				// after setRef() decrement refcnt to 0.
				// That thread is now waiting on the condition variable

				wi.cond.notify_one();
				return;
			}

			wi.objp=0;
			cb_list_cpy.insert(cb_list_cpy.end(),
					   wi.callback_list.begin(),
					   wi.callback_list.end());
			wi.callback_list.clear();

		}

		refadd(-1); // Prevent ref(this) in the destructor
		delete this;

		// Destructor callback invocations must be protected by
		// destroy_mutex. ondestroyObj::cancel()'s contract is that
		// if a destructor callback already in progress, it waits for
		// it to return. After cancel() returns, no callbacks.
		//
		// cancel() takes a lock on destroy_mutex, in order to wait for
		// the following code to finish calling any concurrent
		// callbacks.

		while (!cb_list_cpy.empty())
		{
			cb_list_cpy.front()->destroyed();
			cb_list_cpy.pop_front();
		}

	} catch (const exception &e)
	{
		std::cerr << e << std::endl
			  << e->backtrace << std::flush;

		abort();
	}

}

ptr<weakinfo> obj::weak()
{
	std::lock_guard<std::mutex> weaklock(objmutex);

	if (weakinforef.null())
		weakinforef=ptr<weakinfo>::create(this);

	return weakinforef;
}

std::list< ref<destroyCallbackObj> >::iterator
obj::addOnDestroy(const ref<destroyCallbackObj> &callback)
{
	std::lock_guard<std::mutex> weaklock(objmutex);

	if (weakinforef.null())
		weakinforef=ptr<weakinfo>::create(this);

	weakinfo &wi= *weakinforef;

	std::lock_guard<std::mutex> weakinfolock(wi.mutex);

	wi.callback_list.push_back(callback);

	return --wi.callback_list.end();
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::destroyCallbackObj::destroyed,
		    destroyed_exception);

void destroyCallbackBase::report_exception(const exception &e)
{
	LOG_FUNC_SCOPE(destroyed_exception);

	LOG_ERROR(e);
	LOG_TRACE(e->backtrace);
}
#if 0
{
#endif
}
