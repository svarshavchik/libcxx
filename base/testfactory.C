/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/eventfactory.H"
#include "x/exception.H"
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

int main(int argc, char **argv)
{
	try {
		typedef LIBCXX_NAMESPACE::eventfactory<std::string, int > factory_t;

		factory_t factory(factory_t::create());

		LIBCXX_NAMESPACE::eventregistrationptr r1, r2, r3;

		{
			LIBCXX_NAMESPACE::ptr<inteventhandlerObj>
				ev1=LIBCXX_NAMESPACE::ptr<inteventhandlerObj>::create(),
				ev2=LIBCXX_NAMESPACE::ptr<inteventhandlerObj>::create();

			r1=factory->registerHandler("foo", ev1);
			r2=factory->registerHandler("foo", ev2);
			r3=factory->registerHandler("zzz", ev2);
		}
		factory->event("foo", 1);
		factory->event("bar", 2);

		factory->event(3);

		r1=LIBCXX_NAMESPACE::eventregistrationptr();
		r2=LIBCXX_NAMESPACE::eventregistrationptr();
		r3=LIBCXX_NAMESPACE::eventregistrationptr();

		factory->event("foo", 3);

		factory=factory_t::create();

	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
	return 0;
}
