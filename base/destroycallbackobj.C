/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ondestroy.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

obj::destroyCallbackObj::destroyCallbackObj() noexcept
{
}

obj::destroyCallbackObj::~destroyCallbackObj() noexcept
{
}

onAnyDestroyCallbackObj::cbListObj::cbListObj()
{
}

onAnyDestroyCallbackObj::cbListObj::~cbListObj()
noexcept
{
}

onAnyDestroyCallbackObj::onAnyDestroyCallbackObj(const ref<cbListObj> &cb)
{
	*mpobj<ptr<cbListObj> >::lock(callbacks)=cb;
}

onAnyDestroyCallbackObj::~onAnyDestroyCallbackObj() noexcept
{
}

void onAnyDestroyCallbackObj::destroyed() noexcept
{
	ptr<cbListObj> save;

	{
		mpobj<ptr<cbListObj> >::lock lock(callbacks);

		save= *lock;
		*lock=ptr<cbListObj>();
	}
}

#if 0
{
#endif
}
