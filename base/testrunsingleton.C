/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ref.H"
#include "x/obj.H"
#include "x/exception.H"
#include "x/threads/run.H"
#include <iostream>
#include <cstdlib>
#include <unistd.h>

class xx : virtual public LIBCXX_NAMESPACE::obj,
	   virtual public LIBCXX_NAMESPACE::runthreadsingleton {

public:
	std::mutex m;
	std::condition_variable c;
	bool flag;

	xx() : flag(false) {}
	~xx() {}

	void run()
	{
		std::unique_lock<std::mutex> l(m);

		flag=true;
		c.notify_all();

		c.wait(l, [this] { return !flag; } );
	}
};

int main()
{
	alarm(30);

	auto obj=LIBCXX_NAMESPACE::ref<xx>::create();

	auto first=LIBCXX_NAMESPACE::run(obj);

	{
		std::unique_lock<std::mutex> l(obj->m);

		obj->c.wait(l, [&obj] { return obj->flag; });
	}

	bool caught=false;

	try {
		LIBCXX_NAMESPACE::run(obj);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << "Caught expected exception: " << e << std::endl;
		caught=true;
	}

	{
		std::unique_lock<std::mutex> l(obj->m);

		obj->flag=false;
		obj->c.notify_all();
	}

	first->get();

	if (!caught)
		throw EXCEPTION("Failed to catch an expected exception");
	return 0;
}
