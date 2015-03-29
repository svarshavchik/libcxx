/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/msgdispatcher.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

msgdispatcherObj::msgdispatcherObj()
{
}

msgdispatcherObj::~msgdispatcherObj() noexcept
{
}

class msgdispatcherObj::throwstopexceptionObj : public dispatchablebaseObj {

public:
	throwstopexceptionObj() LIBCXX_HIDDEN;

	~throwstopexceptionObj() noexcept LIBCXX_HIDDEN;

	void dispatch() LIBCXX_HIDDEN;
};

msgdispatcherObj::throwstopexceptionObj::throwstopexceptionObj()
{
}

msgdispatcherObj::throwstopexceptionObj::~throwstopexceptionObj() noexcept
{
}

void msgdispatcherObj::throwstopexceptionObj::dispatch()
{
	throw stopexception();
}

void msgdispatcherObj::stop()
{
	event(ref<throwstopexceptionObj>::create());
}

#if 0
{
#endif
}

