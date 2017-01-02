/*
** Copyright 2012-2016 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "x/threadmsgdispatcher.H"
#include "x/stoppable.H"

#include <cstdlib>
#include <unistd.h>
#include <iostream>

class testclass1 : public LIBCXX_NAMESPACE::threadmsgdispatcherObj {

#include "testmsgdispatcher.testclass1.H"

};

class errorthread : public LIBCXX_NAMESPACE::threadmsgdispatcherObj {

#include "testmsgdispatcher.testclass2.H"

};

void testclass1::dispatch_method2(const char *strptr,
				  const std::string &objectarg)
{
}

void testclass1::dispatch_method1(const char *strptr)
{
}

void testclass1::dispatch_method0()
{
}

void errorthread::dispatch_logerror(const char *file,
				    int line,
				    const std::string &error_message)
{
}

void errorthread::dispatch_report_errors(const x::ptr<x::obj> &mcguffin,
					 void (*callback_func)(const char *file,
							       int line,
							       const std::string &errmsg))
{
}

void errorthread::dispatch_dump(const std::string &filename,
				int count,
				int repeat,
				int errcode)
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

static LIBCXX_NAMESPACE::mpobj<bool> flag{false};

void mythreadObj::run(LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj> &mcguffin,
		      int ignored)
{
	msgqueue_auto q{ this, LIBCXX_NAMESPACE::eventfd::create() };

	sleep(2);
	{
		LIBCXX_NAMESPACE::mpobj<bool>::lock lock(flag);

		*lock=true;
	}

	mcguffin=nullptr;

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

	auto ret=LIBCXX_NAMESPACE::start_threadmsgdispatcher(t, 1);

	{
		LIBCXX_NAMESPACE::mpobj<bool>::lock lock(flag);

		if (!*lock)
		{
			throw EXCEPTION("This is wrong");
		}
	}

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
		_exit(1);
	}
	return 0;
}
