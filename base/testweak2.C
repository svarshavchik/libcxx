/*
** Copyright 2012-2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ptr.H"
#include "x/obj.H"
#include "x/weakmultimap.H"
#include "x/weakmap.H"
#include "x/ondestroy.H"

#include <string>
#include <iostream>
#include <unistd.h>
#include <vector>

class valueObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	int n;

	valueObj(int nArg=0):n(nArg) {}
	~valueObj() noexcept {}
};

typedef LIBCXX_NAMESPACE::weakmultimap<std::string, valueObj> mmap_t;
typedef LIBCXX_NAMESPACE::weakmap<std::string, valueObj> map_t;

class myDestructorCallbackObj : public LIBCXX_NAMESPACE::destroyCallbackObj {

	mmap_t &m;

public:
	myDestructorCallbackObj(mmap_t &mArg) noexcept;
	~myDestructorCallbackObj() noexcept;
	void destroyed() noexcept;
};

myDestructorCallbackObj::myDestructorCallbackObj(mmap_t &mArg) noexcept
	: m(mArg)
{
}

myDestructorCallbackObj::~myDestructorCallbackObj() noexcept
{
}

void myDestructorCallbackObj::destroyed() noexcept
{
	mmap_t::base::iterator b(m->begin()), e(m->end());
	while (b != e)
	{
		std::cout << "weakmap: " << b->first << ": "
			  << (b->second.getptr().null() ? "null":"not null")
			  << std::endl;
		++b;
	}
}

template<typename t>
void find_or_create_must_return_a_ref();

template<>
void find_or_create_must_return_a_ref<LIBCXX_NAMESPACE::ref<valueObj>>()
{
}

void test_find_or_create()
{
	mmap_t m=mmap_t::create();

	auto a=LIBCXX_NAMESPACE::ptr<valueObj>::create(0);
	auto b=LIBCXX_NAMESPACE::ptr<valueObj>::create(1);

	m->insert("A", a);
	m->insert("B", b);

	auto p=m->begin();

	if (p->second.getptr()->n != 0)
		throw EXCEPTION("find_or_create test 1 failed");

	auto lambda=[]
		{
			return LIBCXX_NAMESPACE::ref<valueObj>::create(2);
		};

	if (m->find_or_create("B", lambda)->n != 1)
		throw EXCEPTION("find_or_create test 2 failed");

	b=LIBCXX_NAMESPACE::ptr<valueObj>();

	if (m->count("B") != 1)
		throw EXCEPTION("find_or_create test 3 failed");

	auto c=m->begin();

	if (c == m->end() || ++c == m->end() || c->first != "B")
		throw EXCEPTION("find_or_create test 4 failed");

	if (m->find_or_create("B", lambda)->n != 2)
		throw EXCEPTION("find_or_create test 5 failed");

	find_or_create_must_return_a_ref
		<decltype(m->find_or_create("B", lambda))>();
}

void test_find_or_create2()
{
	map_t m=map_t::create();

	auto first=m->find_or_create("A",
				     []
				     {
					     return LIBCXX_NAMESPACE
						     ::ref<valueObj>::create(2);

				     });

	std::cout << "first: " << first.null() << std::endl;
}

int main(int argc, char **argv)
{
	try {
		alarm(60);

		{
			mmap_t m(mmap_t::create());

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
			mmap_t m(mmap_t::create());

			LIBCXX_NAMESPACE::ptr<valueObj> v(LIBCXX_NAMESPACE::ptr<valueObj>::create());

			{
				m->insert(std::make_pair(std::string("B"), v));

				LIBCXX_NAMESPACE::ondestroy::create
					(LIBCXX_NAMESPACE::ref<myDestructorCallbackObj>
					 ::create(m), v);
			}
			v=LIBCXX_NAMESPACE::ptr<valueObj>();
		}
		test_find_or_create();
		test_find_or_create2();
	} catch (LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
	}
	return 0;
}


