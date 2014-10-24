/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/destroycallbackobj.H"
#include "x/ondestroy.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

destroyCallbackObj::destroyCallbackObj() noexcept
{
}

destroyCallbackObj::~destroyCallbackObj() noexcept
{
}

destroyCallbackObj::onAnyDestroyCallbackObj::cbListObj::cbListObj()
{
}

destroyCallbackObj::onAnyDestroyCallbackObj::cbListObj::~cbListObj() noexcept
{
}

destroyCallbackObj::onAnyDestroyCallbackObj
::onAnyDestroyCallbackObj(const ref<cbListObj> &cb)
{
	*mpobj<ptr<cbListObj> >::lock(callbacks)=cb;
}

destroyCallbackObj::onAnyDestroyCallbackObj::~onAnyDestroyCallbackObj() noexcept
{
}

void destroyCallbackObj::onAnyDestroyCallbackObj::destroyed() noexcept
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
