/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_threads_weaklocal_H
#define x_threads_local_H

#include <x/namespace.h>
#include <x/threads/weaklocalfwd.H>
#include <x/threads/weaklocalobj.H>
#include <x/threads/run.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

#ifndef DOXYGEN_FIXME

template<typename objClass, typename baseType>
weakthreadlocal<objClass, baseType>
run_async::makeweakthreadlocal(const ref<objClass, baseType> &refobj)
{
	if (!localscope::localscopeptr)
		no_local_variables();
			
	return weakthreadlocal<objClass, baseType>
		::create(refobj, localscope::localscopeptr->mcguffin);
}

#endif

template<typename objClass, typename baseType>
inline weakthreadlocal<objClass, baseType>
makeweakthreadlocal(const ref<objClass, baseType> &refobj)
{
	return run_async::makeweakthreadlocal(refobj);
}

template<typename objClass, typename baseType>
inline weakthreadlocal<objClass, baseType>
makeweakthreadlocal(const ptr<objClass, baseType> &refobj)
{
	return run_async::makeweakthreadlocal(ref<objClass, baseType>(refobj));
}

#if 0
{
#endif
}

#endif
