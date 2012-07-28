/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "weakmultimap.H"
#include "ptr.H"
#include "obj.H"
#include "ondestroy.H"

#include <string>
#include <iostream>
#include <unistd.h>

class valueObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	valueObj() noexcept {}
	~valueObj() noexcept {}
};

typedef LIBCXX_NAMESPACE::weakmultimap<std::string, valueObj> map_t;

class myDestructorCallbackObj : public LIBCXX_NAMESPACE::destroyCallbackObj {

	map_t &m;

public:
	myDestructorCallbackObj(map_t &mArg) noexcept;
	~myDestructorCallbackObj() noexcept;
	void destroyed() noexcept;
};

myDestructorCallbackObj::myDestructorCallbackObj(map_t &mArg) noexcept
	: m(mArg)
{
}

myDestructorCallbackObj::~myDestructorCallbackObj() noexcept
{
}

void myDestructorCallbackObj::destroyed() noexcept
{
	map_t::base::iterator b(m->begin()), e(m->end());
	while (b != e)
	{
		std::cout << "weakmap: " << b->first << ": "
			  << (b->second.getptr().null() ? "null":"not null")
			  << std::endl;
		++b;
	}
}

int main(int argc, char **argv)
{
	try {
		alarm(60);

		{
			map_t m(map_t::create());

			LIBCXX_NAMESPACE::ptr<valueObj> v(LIBCXX_NAMESPACE::ptr<valueObj>::create());

			{
				LIBCXX_NAMESPACE::ondestroy::create
					(LIBCXX_NAMESPACE::ptr<myDestructorCallbackObj>
					 ::create(m),
					 v);

				m->insert(std::make_pair(std::string("A"), v));
			}
			v=LIBCXX_NAMESPACE::ptr<valueObj>();
		}

		{
			map_t m(map_t::create());

			LIBCXX_NAMESPACE::ptr<valueObj> v(LIBCXX_NAMESPACE::ptr<valueObj>::create());

			{
				m->insert(std::make_pair(std::string("B"), v));

				LIBCXX_NAMESPACE::ondestroy::create
					(LIBCXX_NAMESPACE::ref<myDestructorCallbackObj>
					 ::create(m), v);
			}
			v=LIBCXX_NAMESPACE::ptr<valueObj>();
		}

	} catch (LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
	}
	return 0;
}


