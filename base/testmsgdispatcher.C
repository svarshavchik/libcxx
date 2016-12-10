/*
** Copyright 2012-2016 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "x/msgdispatcher.H"
#include "x/threadmsgdispatcher.H"
#include "x/stoppable.H"

#include <cstdlib>
#include <unistd.h>
#include <iostream>

class testclass1 : public LIBCXX_NAMESPACE::msgdispatcherObj {

#include "testmsgdispatcher.testclass1.all.H"

};

class errorthread : public LIBCXX_NAMESPACE::msgdispatcherObj {

#include "testmsgdispatcher.testclass2.decl.H"

};

#include "testmsgdispatcher.testclass2.def.H"

void testclass1::dispatch(const method2_msg &msg)

{
}

void testclass1::dispatch(const method1_msg &msg)

{
}

void testclass1::dispatch(const method0_msg &msg)

{
}

void errorthread::dispatch(const logerror_msg &msg)
{
}

void errorthread::dispatch(const report_errors_msg &msg)
{
}

void errorthread::dispatch(const dump_msg &msg)
{
}

class mythreadObj;

static int copy_counter=0;

class test_copies {

public:
	test_copies() = default;

	test_copies(const test_copies &)
	{
		++copy_counter;
	}

	test_copies &operator=(const test_copies &)=delete;
};

class mythreadObj : public LIBCXX_NAMESPACE::threadmsgdispatcherObj {


public:
	void run(LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj> &mcguffin,
		 int arg);

	void message1(test_copies &c)
	{
	}

	class testas_class {
	public:
		testas_class(int) {}
	};

#include "threadmsgdispatcher.H"
};

void mythreadObj::dispatch_logerror(const char *file, int line,
				    const std::string &msg) noexcept
{
}

void mythreadObj::dispatch_testas(testas_class param)
{
}

void mythreadObj::run(LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj> &mcguffin,
		      int ignored)
{
	msgqueue_auto q{ this, LIBCXX_NAMESPACE::eventfd::create() };

	mcguffin=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>();

	try {
		while (1)
			q.event();
	} catch (const LIBCXX_NAMESPACE::stopexception &e)
	{
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
}

void testthreadmsgdispatcher()
{
	auto t=LIBCXX_NAMESPACE::ref<mythreadObj>::create();

	auto ret=LIBCXX_NAMESPACE::start_thread(t, 1);

	test_copies c;

	t->logerror("Foo", 1, "bar");
	t->logerror();
	t->testas(42.0);
	t->sendevent(&mythreadObj::message1, t, c);
	t->stop();

	ret->wait();

	if (copy_counter != 1)
	{
		std::cerr << "Did not expect " << copy_counter << " copies."
			  << std::endl;
		exit(1);
	}
}

int main(int argc, char **argv)
{
	alarm(30);
	try {
		testthreadmsgdispatcher();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
