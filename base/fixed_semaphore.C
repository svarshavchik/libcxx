/*
** Copyright 2014-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fixed_semaphore.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::fixed_semaphoreObj);

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

fixed_semaphoreObj::fixed_semaphoreObj(size_t howmany)
	: available(howmany)
{
}

fixed_semaphoreObj::~fixed_semaphoreObj()
{
}

fixed_semaphoreObj::callbackBaseObj::callbackBaseObj()
{
}

fixed_semaphoreObj::callbackBaseObj::~callbackBaseObj()
{
}

fixed_semaphore::base::acquiredptr
fixed_semaphoreObj::do_acquire(size_t howmany,
			       const ref<callbackBaseObj> &callback)
{
	auto optimist=fixed_semaphore::base::acquired
		::create(ref<fixed_semaphoreObj>(this), callback);

	mpobj<size_t>::lock lock(available);

	if (howmany > *lock || !callback->acquired())
		return fixed_semaphore::base::acquiredptr();

	*lock -= howmany;
	optimist->howmany=howmany;
	optimist->acquired=true;
	return optimist;
}

fixed_semaphoreObj::acquiredObj
::acquiredObj(const ref<fixed_semaphoreObj> acquired_fromArg,
	      const ref<callbackBaseObj> callbackArg)
	: howmany(0), acquired(false), acquired_from(acquired_fromArg),
	  callback(callbackArg)
{
}

fixed_semaphoreObj::acquiredObj::~acquiredObj()
{
	if (!acquired)
		return;

	try {
		callback->release();
	} catch (const exception &e)
	{
		LOG_ERROR(e);
		LOG_TRACE(e->backtrace);
	}

	mpobj<size_t>::lock lock(acquired_from->available);

	*lock += howmany;
}

#if 0
{
#endif
}
