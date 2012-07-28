/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include <iostream>
#include <string>
#include <unistd.h>

#define LIBCXX_WEAKDEBUG_HOOK(x) std::cout << x << std::endl

#include "weaklist.H"
#include "ptr.H"
#include "obj.H"

class valueObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	valueObj() noexcept {}
	~valueObj() noexcept {}
};

typedef LIBCXX_NAMESPACE::weaklist<valueObj> list_t;

int main(int argc, char **argv)
{
	try {
		alarm(60);

		list_t l(list_t::create());

		LIBCXX_NAMESPACE::ptr<valueObj> v(LIBCXX_NAMESPACE::ptr<valueObj>::create());

		{
			std::cout << "size=" << l->size() << std::endl;

			l->push_back(LIBCXX_NAMESPACE::ref<valueObj>(v));

			std::cout << "size=" << l->size() << std::endl;

			v=LIBCXX_NAMESPACE::ptr<valueObj>();

			std::cout << "size=" << l->size() << std::endl;
		}

		v=LIBCXX_NAMESPACE::ptr<valueObj>::create();

		{
			l->push_front(v);

			std::cout << "size=" << l->size() << std::endl;

			list_t::base::iterator b(l->begin()), e(l->end());

			v=LIBCXX_NAMESPACE::ptr<valueObj>();

			std::cout << "size=" << l->size() << std::endl;
		}
		std::cout << "size=" << l->size() << std::endl;

	} catch (LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
	}
	return 0;
}


