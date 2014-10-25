/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/stoppable.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

stoppableObj::stoppableObj()
{
}

stoppableObj::~stoppableObj() noexcept
{
}

class LIBCXX_HIDDEN stoppableBase::groupObj::cbObj : virtual public obj {

 public:
	weaklist<stoppableObj> stoppables;

	cbObj(const weaklist<stoppableObj> &stoppablesArg)
		: stoppables(stoppablesArg)
	{
	}

	~cbObj() noexcept
	{
	}

	void destroyed()
	{
		for (auto s: *stoppables)
		{
			auto sptr=s.getptr();

			if (!sptr.null())
				sptr->stop();
		}
	}
};

stoppableBase::groupObj::groupObj()
	: stoppables(weaklist<stoppableObj>::create())
{
}

stoppableBase::groupObj::~groupObj() noexcept
{
}

void stoppableBase::groupObj::add(const stoppable &member)
{
	stoppables->push_back(member);
}

void stoppableBase::groupObj::mcguffin(const ref<obj> &mcguffin)
{
	auto s=ref<cbObj>::create(stoppables);

	mcguffin->ondestroy([s] { s->destroyed(); });
}

#if 0
{
#endif
}
