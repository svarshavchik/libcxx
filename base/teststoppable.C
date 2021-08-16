/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"

#include "x/stoppable.H"
#include "x/mpobj.H"
#include "x/mpobj.H"

#include <unistd.h>
#include <iostream>

class amIstoppedObj : public LIBCXX_NAMESPACE::stoppableObj {

public:

	LIBCXX_NAMESPACE::mpcobj<bool> flag;

	amIstoppedObj() : flag(false) {}
	~amIstoppedObj() {}

	void stop() override
	{
		LIBCXX_NAMESPACE::mpcobj<bool>::lock lock(flag);

		*lock=true;
		lock.notify_all();
	}
};

typedef LIBCXX_NAMESPACE::ptr<amIstoppedObj> amIstopped;

void test1()
{
	amIstopped obj1=amIstopped::create(), obj2=amIstopped::create();

	{
		auto group=LIBCXX_NAMESPACE::stoppable::base::group::create();

		group->add(obj1);
		group->add(obj2);

		group->mcguffin(obj1);

		typedef LIBCXX_NAMESPACE::mpcobj<bool> flag_t;

		flag_t flag(false);

		obj1->ondestroy([&flag]
				{
					{
						flag_t::lock lock(flag);

						*lock=true;
						lock.notify_one();

						std::cout <<
							"obj1 destroyed"
							  << std::endl;
					}
				});

		obj1=amIstopped();

		flag_t::lock lock(flag);

		lock.wait([&lock]
			  {
				  return *lock;
			  });
	}

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock(obj2->flag);

	lock.wait([&lock] { return *lock; });
}

int main(int argc, char **argv)
{
	alarm(30);

	test1();
	return 0;
}
