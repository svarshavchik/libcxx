/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/fd.H"

static LIBCXX_NAMESPACE::fdptr r, w;

#define LIBCXX_EVENTFACTORY_IMPL_DEBUGHOOK1() do {			\
		std::cout << "Closing the write side of the pipe"	\
			  << std::endl;					\
	} while(0)

#define LIBCXX_EVENTFACTORY_IMPL_DEBUGHOOK2() do {			\
		w=LIBCXX_NAMESPACE::fdptr(); sleep(3); std::cout << "Handler removed"	\
			  << std::endl;					\
	} while(0)

#define LIBCXX_EVENTFACTORY_EVENT_DEBUGHOOK() do {			\
		std::cout << "In event()" << std::endl;			\
		} while(0)

#include "x/eventfactory.H"
#include "x/exception.H"
#include "x/threads/run.H"
#include <iostream>


class inteventhandlerObj : public LIBCXX_NAMESPACE::eventhandlerObj<int> {

public:
	inteventhandlerObj() noexcept {}
	~inteventhandlerObj() {}

	void event(const int &n) override
	{
		std::cout << "Event " << n << std::endl;
	}
};

class myThread : virtual public LIBCXX_NAMESPACE::obj {

	LIBCXX_NAMESPACE::eventregistrationptr *rObj;

public:
	myThread(LIBCXX_NAMESPACE::eventregistrationptr *rObjArg)
		: rObj(rObjArg)
	{
	}

	~myThread()
	{
	}

	void run();
};


void myThread::run()
{
	std::cout << "Destroying event handler"
		  << std::endl;
	*rObj=LIBCXX_NAMESPACE::eventregistrationptr();
}

static void testfactory2()
{
	std::cout << "Test factory 2" << std::endl;

	{
		std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
			p=LIBCXX_NAMESPACE::fd::base::pipe();

		r=p.first;
		w=p.second;
	}

	typedef LIBCXX_NAMESPACE::eventfactory<std::string, int > factory_t;

	{
		factory_t factory(factory_t::create());

		LIBCXX_NAMESPACE::ptr<inteventhandlerObj>
			ev=LIBCXX_NAMESPACE::ptr<inteventhandlerObj>::create();

		LIBCXX_NAMESPACE::eventregistrationptr er=
			factory->registerHandler("foo", ev);

		std::cout << "Starting subthread" << std::endl;

		LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE::ref<myThread>
				    ::create(&er));

		char buffer[1];

		r->read(buffer, 1);
		std::cout << "Write side of the pipe is closed"
			  << std::endl;
		factory->event("foo", 1);
	}
	std::cout << "Factory destroyed" << std::endl;
}

static volatile bool testThreadFlag;

template<bool wait>
class testThreadDestr : virtual public LIBCXX_NAMESPACE::obj {

public:
	testThreadDestr()
	{
	}

	~testThreadDestr()
	{
		if (wait)
		{
			std::cout << "testthread: destructor start"
				  << std::endl;

			sleep(3);
			std::cout << "testthread: destructor end" << std::endl;
		}
		testThreadFlag=true;
	}

	void run()
	{
		if (wait)
			std::cout << "testthread: run()" << std::endl;
	}
};

static void testthreaddestr()
{
	testThreadFlag=false;

	std::cout << "testthread: starting thread" << std::endl;

	LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE::ref<testThreadDestr<true> >
			    ::create())->get();

	if (!testThreadFlag)
		throw EXCEPTION("Thread destruction failure");

	for (size_t i=0; i<1000; i++)
	{
		testThreadFlag=false;

		LIBCXX_NAMESPACE::run(LIBCXX_NAMESPACE::ref<testThreadDestr<false> >
				    ::create())->get();

		if (!testThreadFlag)
			throw EXCEPTION("Thread destruction failure");
	}
}

int main(int argc, char **argv)
{
	alarm(30);

	try {
		testfactory2();
		testthreaddestr();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
	return 0;
}
