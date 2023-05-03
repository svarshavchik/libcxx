/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ptr.H"
#include "x/obj.H"
#include "x/weakmultimap.H"
#include "x/weakmap.H"
#include "x/weakunordered_map.H"
#include "x/weakunordered_multimap.H"
#include <x/weakptr.H>
#include "x/ondestroy.H"

#include <string>
#include <iostream>
#include <unistd.h>
#include <vector>

class valueObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	int n;

	valueObj(int nArg=0):n(nArg) {}
	~valueObj() {}
};

typedef LIBCXX_NAMESPACE::weakmultimap<std::string, valueObj> mmap_t;
typedef LIBCXX_NAMESPACE::weakmap<std::string, valueObj> map_t;

static void dump_m(const mmap_t &m)
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

void testunordered1()
{
	auto m=LIBCXX_NAMESPACE::weakunordered_map<int, LIBCXX_NAMESPACE::obj>
		::create();

	auto obj1=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>::create();
	auto obj2=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>::create();

	if (!m->insert(1, obj1))
		throw EXCEPTION("testunordered1 (1) failed");

	auto p=m->begin();

	if (p == m->end() || p->first != 1 || ++p != m->end())
		throw EXCEPTION("testunordered1 (2) failed");

	if (m->insert(1, obj2) ||
	    !m->insert(2, obj2))
		throw EXCEPTION("testunordered1 (3) failed");

	if ((!((*m)[3]=obj2)))
		throw EXCEPTION("testunordered1 (4) failed");

	if (m->find_or_create(1, [&]
			      {
				      return obj1;
			      }).null()
	    || m->find_or_create(4, [&]
				  {
					  return obj1;
				  }).null())
		throw EXCEPTION("testunordered1 (5) failed");
}

void testunordered2()
{
	auto m=LIBCXX_NAMESPACE::weakunordered_multimap<int, LIBCXX_NAMESPACE::obj>
		::create();

	auto obj1=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>::create();
	auto obj2=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>::create();

	if (!m->insert(1, obj1))
		throw EXCEPTION("testunordered2 (1) failed");

	auto p=m->begin();

	if (p == m->end() || p->first != 1 || ++p != m->end())
		throw EXCEPTION("testunordered2 (2) failed");

	if (!m->insert(1, obj2) ||
	    !m->insert(2, obj2))
		throw EXCEPTION("testunordered2 (3) failed");

	if ((!((*m)[3]=obj2)))
		throw EXCEPTION("testunordered2 (4) failed");

	m->find_or_create(1, [&]
			  {
				  return obj1;
			  });
}

bool selfdestroy1_created=false;

struct selfdestroy1Obj : virtual public LIBCXX_NAMESPACE::obj {

	typedef LIBCXX_NAMESPACE::weakmultimap<int, LIBCXX_NAMESPACE::obj> mm_t;

	mm_t mm=mm_t::create();

	~selfdestroy1Obj()
	{
		mm->find_or_create(
			0,
			[&]
			{
				selfdestroy1_created=true;

				return LIBCXX_NAMESPACE::ref<
					LIBCXX_NAMESPACE::obj>::create();
			});
	}
};

void testselfdestroy1()
{
	auto p=LIBCXX_NAMESPACE::ref<selfdestroy1Obj>::create();

	p->mm->insert(0, p);

	p=LIBCXX_NAMESPACE::ref<selfdestroy1Obj>::create();

	if (!selfdestroy1_created)
		throw EXCEPTION("testselfdestroy1 failed");
}


bool selfdestroy2_created=false;

struct selfdestroy2Obj : virtual public LIBCXX_NAMESPACE::obj {

	LIBCXX_NAMESPACE::weakptr<LIBCXX_NAMESPACE::ptr<selfdestroy2Obj>> me;

	~selfdestroy2Obj()
	{
		if (me.getptr().null())
			selfdestroy2_created=true;
	}
};

void testselfdestroy2()
{
	auto p=LIBCXX_NAMESPACE::ref<selfdestroy2Obj>::create();

	p->me=p;

	p=LIBCXX_NAMESPACE::ref<selfdestroy2Obj>::create();

	if (!selfdestroy2_created)
		throw EXCEPTION("testselfdestroy2 failed");
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
					([m]
					 {
						 dump_m(m);
					 }, v);

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
					([m]
					 {
						 dump_m(m);
					 }, v);
			}
			v=LIBCXX_NAMESPACE::ptr<valueObj>();
		}
		test_find_or_create();
		test_find_or_create2();
		testselfdestroy1();
		testselfdestroy2();

		testunordered1();
		testunordered2();
	} catch (LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
	}
	return 0;
}
