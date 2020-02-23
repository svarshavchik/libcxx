/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ref.H"
#include "x/obj.H"
#include "x/exception.H"
#include "x/threads/run.H"
#include "x/threads/runthreadname.H"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

class xx {

public:
	xx() { std::cout << "Constructed " << this << std::endl; }
	~xx() { std::cout << "Destroyed " << this << std::endl; }

	xx(const xx &y)
	{
		std::cout << "Constructed " << this << " from " << &y
			  << std::endl;
	}

	xx &operator=(const xx &y)
	{
		std::cout << "Assigned " << this << " from " << &y
			  << std::endl;
		return *this;
	}
};

class foo : virtual public LIBCXX_NAMESPACE::obj,
	    public LIBCXX_NAMESPACE::runthreadname {

public:
	foo() {}
	~foo() {}

	xx run(xx &xp)
	{
		std::cout << "Executed foo with " << &xp << std::endl;

		sleep(1);

		xx ret;

		std::cout << "Return value is " << &ret << std::endl;

		return ret;
	}

	void run()
	{
		sleep(2);
	}

	int run(const std::string &exception_text)
	{
		throw EXCEPTION(exception_text);
	}

	void run(const std::string &exception_test, int ignore)
	{
		throw EXCEPTION(exception_test);
	}

	std::string getName() const override
	{
		return "foo";
	}
};

static LIBCXX_NAMESPACE::runthread<xx> mkthread(const LIBCXX_NAMESPACE::ref<foo> &foor,
						     const xx &param)
{
	return LIBCXX_NAMESPACE::run(foor, param);
}

int main()
{
	LIBCXX_NAMESPACE::ref<foo> foor=LIBCXX_NAMESPACE::ref<foo>::create();
	LIBCXX_NAMESPACE::ptr<foo> foop;

	xx param;

	std::cout << "Parameter is " << &param << std::endl;

	LIBCXX_NAMESPACE::runthread<xx> ret = mkthread(foor, param);

	std::cout << "Terminated: " << ret->terminated() << std::endl;

	ret->wait();

	xx obj=ret->get();

	std::cout << "Main returned " << &obj << std::endl;

	LIBCXX_NAMESPACE::runthread<int> exception_call=LIBCXX_NAMESPACE::run(foor, "rosebud");

	bool caught=false;
	try {
		int n=exception_call->get();


		std::cout << "How did a thrown exception return "
			  << n << "?" << std::endl;
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << "Rethrown exception: " << e << std::endl;
		caught=true;
	}

	if (!caught)
		exit(1);

	if (LIBCXX_NAMESPACE::runthreadname::name<foo>::get(*foor)
	    != "foo")
		throw EXCEPTION("runthreadname::get() does not work");

	std::cout << LIBCXX_NAMESPACE::runthreadname::name<decltype(foor)>::get(foor)
		  << std::endl;
	foop=foor;

	LIBCXX_NAMESPACE::runthread<void> ret2=run(foop);

	caught=false;

	try {
		run(foop, "void run() exception", 1)->get();
	} catch (...)
	{
		caught=true;
	}

	if (!caught)
		throw EXCEPTION("get() did not propagate exceptions from runthread<void>");

	return 0;
}
