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

msgdispatcherObj::~msgdispatcherObj()
{
}

class msgdispatcherObj::throwstopexceptionObj : public dispatchablebaseObj {

public:
	throwstopexceptionObj() LIBCXX_HIDDEN;

	~throwstopexceptionObj() LIBCXX_HIDDEN;

	void dispatch() override LIBCXX_HIDDEN;
};

msgdispatcherObj::throwstopexceptionObj::throwstopexceptionObj()
{
}

msgdispatcherObj::throwstopexceptionObj::~throwstopexceptionObj()
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
