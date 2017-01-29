/*
** Copyright 2012-2014 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

static bool dumpflag=true;

#define LIBCXX_WEAKDEBUG_HOOK(x) do {		\
		if (dumpflag)			\
			std::cout << x << std::endl;	\
	} while(0);					\

#include "x/weaklist.H"
#include "x/mcguffinlist.H"
#include "x/mcguffinmap.H"
#include "x/mcguffinmultimap.H"
#include "x/mcguffinunordered_map.H"
#include "x/mcguffinunordered_multimap.H"
#include "x/ptr.H"
#include "x/obj.H"

class valueObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	valueObj() noexcept {}
	int v_proof=1;
	~valueObj() {}
};

typedef LIBCXX_NAMESPACE::weaklist<valueObj> list_t;

static void test1()
{
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
}

typedef LIBCXX_NAMESPACE::mcguffinlist<LIBCXX_NAMESPACE::ref<valueObj>> mlist_t;

static void test2()
{
	dumpflag=false;

	auto l=mlist_t::create();

	auto v=LIBCXX_NAMESPACE::ref<valueObj>::create();

	{
		auto mcguffin=l->push_front(v);

		std::cout << "test2: size=" << l->size() << std::endl;

		for (const auto &iter:*l)
		{
			LIBCXX_NAMESPACE::ptr<valueObj> p=
				iter.getptr();

			std::cout << "null: " << p.null() << std::endl;
		}
	}

	std::cout << "test2: size=" << l->size() << std::endl;
	for (const auto &iter:*l)
	{
		LIBCXX_NAMESPACE::ptr<valueObj> p=iter.getptr();

		std::cout << "null: " << p.null() << std::endl;
	}
	{
		auto mcguffin=l->push_back(v);

		std::cout << "test2: size=" << l->size() << std::endl;

		for (auto &iter:*l)
		{
			LIBCXX_NAMESPACE::ptr<valueObj> p=
				iter.getptr();

			std::cout << "null: " << p.null() << std::endl;
			iter.erase();
			iter.erase();
			p=iter.getptr();
			std::cout << "null after erase: "
				  << p.null() << std::endl;
		}

		for (auto &iter:*l)
		{
			LIBCXX_NAMESPACE::ptr<valueObj> p=iter.getptr();

			std::cout << "null: " << p.null() << std::endl;
			iter.erase();
		}
	}
	{
		auto mcguffin=l->push_front(v);

		std::cout << "test2: size=" << l->size() << std::endl;
	}

	std::cout << "test2: size=" << l->size() << std::endl;
	for (auto &iter:*l)
	{
		LIBCXX_NAMESPACE::ptr<valueObj> p=iter.getptr();

		std::cout << "null: " << p.null() << std::endl;
		iter.erase();
	}
}

template<typename map_t>
static void test34_common(const char *testname)
{
	auto m=map_t::create();

	auto v=LIBCXX_NAMESPACE::ref<valueObj>::create();

	{
		auto mcguffin=m->insert(std::make_pair(0, v));

		std::cout << testname << "insert mcguffin null: "
			  << mcguffin.null() << std::endl;

		for (const auto &v:*m)
		{
			std::cout << testname << "key " << v.first
				  << std::flush
				  << " value: " << v.second.getptr()->v_proof
				  << std::endl;
			v.second.erase();

			std::cout << testname << "key " << v.first
				  << " after remove, null: "
				  << v.second.getptr().null()
				  << std::endl;
		}
		for (const auto &v:*m)
		{
			std::cout << testname << "key " << v.first
				  << " shouldn't be here." << std::endl;
		}
	}

	{
		auto mcguffin=(*m)[0]=v;

		std::cout << testname << "size is " << m->size() << std::endl;
		if (m->find(0) == m->end())
			std::cout << "find(0) equal to end()"
				  << std::endl;
		for (const auto &v:*m)
		{
			std::cout << testname << "key " << v.first
				  << std::flush
				  << " value: " << v.second.getptr()->v_proof
				  << std::endl;
		}
	}
	std::cout << testname << "size is " << m->size() << std::endl;

	for (const auto &v:*m)
	{
		std::cout << testname << "ghost key " << v.first
			  << std::flush
			  << " value: " << v.second.getptr()->v_proof
			  << std::endl;
	}

	m->count(0);
	m->equal_range(0);
}

static void test3()
{
	typedef LIBCXX_NAMESPACE::mcguffinmap
		<int, LIBCXX_NAMESPACE::ref<valueObj>> map_t;

	test34_common<map_t>("test3: ");

	auto m=map_t::create();

	m->lower_bound(0);
	m->upper_bound(0);

	{
		std::vector<LIBCXX_NAMESPACE::ref<valueObj>> v;

		v.push_back(LIBCXX_NAMESPACE::ref<valueObj>::create());

		auto mcguffin=m->insert(std::make_pair(0, v.front()));

		std::cout << "test3: this should be 1: "
			  << m->insert(std::make_pair(0, v.front())).null()
			  << std::endl;

		std::cout << "test3: this should be 0: "
			  << m->insert(std::make_pair(1, v.front())).null()
			  << std::endl;
		v.clear();

		for (const auto &v:*m)
		{
			std::cout << "test3: remaining key " << v.first
				  << std::flush
				  << " value: " << v.second.getptr()->v_proof
				  << std::endl;
		}

	}
}

static void test3u()
{
	typedef LIBCXX_NAMESPACE::mcguffinunordered_map
		<int, LIBCXX_NAMESPACE::ref<valueObj>> map_t;

	test34_common<map_t>("test3u: ");

	auto m=map_t::create();

	{
		std::vector<LIBCXX_NAMESPACE::ref<valueObj>> v;

		v.push_back(LIBCXX_NAMESPACE::ref<valueObj>::create());

		auto mcguffin=m->insert(std::make_pair(0, v.front()));

		std::cout << "test3u: this should be 1: "
			  << m->insert(std::make_pair(0, v.front())).null()
			  << std::endl;

		std::cout << "test3u: this should be 0: "
			  << m->insert(std::make_pair(1, v.front())).null()
			  << std::endl;
		v.clear();

		for (const auto &v:*m)
		{
			std::cout << "test3u: remaining key " << v.first
				  << std::flush
				  << " value: " << v.second.getptr()->v_proof
				  << std::endl;
		}

	}
}

static void test4()
{
	typedef LIBCXX_NAMESPACE::mcguffinmultimap
		<int, LIBCXX_NAMESPACE::ref<valueObj>> map_t;

	test34_common<map_t>("test4: ");

	auto m=map_t::create();

	m->lower_bound(0);
	m->upper_bound(0);

	{
		std::vector<LIBCXX_NAMESPACE::ref<valueObj>> v;

		v.push_back(LIBCXX_NAMESPACE::ref<valueObj>::create());

		auto mcguffin=m->insert(std::make_pair(0, v.front()));

		std::cout << "test4: this should be 0: "
			  << m->insert(std::make_pair(0, v.front())).null()
			  << std::endl;

		std::cout << "test4: this should be 0: "
			  << m->insert(std::make_pair(1, v.front())).null()
			  << std::endl;
		v.clear();

		for (const auto &v:*m)
		{
			std::cout << "test4: remaining key " << v.first
				  << std::flush
				  << " value: " << v.second.getptr()->v_proof
				  << std::endl;
		}
	}

	std::vector<LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>> save_mcguffin;

	{
		auto v=LIBCXX_NAMESPACE::ref<valueObj>::create();

		auto mcguffin1=m->find_or_create(0, [&v]
						 {
							 return v;
						 });

		auto mcguffin2=m->find_or_create(0, [&v]
						 {
							 return v;
						 });

		for (const auto &v:*m)
		{
			std::cout << "test4: after find_or_create: " << v.first
				  << std::flush
				  << " value: " << v.second.getptr()->v_proof
				  << std::endl;

			auto p=v.second.mcguffin();

			std::cout << "test4: mcguffin.null() should be 0: "
				  << p.null() << std::endl;
			save_mcguffin.push_back(p);
		}
	}

	for (const auto &v:*m)
	{
		std::cout << "test4: mcguffin should exist: " << v.first
			  << std::flush
			  << " value: " << v.second.getptr()->v_proof
				  << std::endl;

		save_mcguffin.clear();

		std::cout << "test4: mcguffin should work: " << v.first
			  << std::flush
			  << " null: " << v.second.getptr().null()
			  << std::endl;
	}
}

static void test4u()
{
	typedef LIBCXX_NAMESPACE::mcguffinmultimap
		<int, LIBCXX_NAMESPACE::ref<valueObj>> map_t;

	test34_common<map_t>("test4u: ");

	auto m=map_t::create();

	{
		std::vector<LIBCXX_NAMESPACE::ref<valueObj>> v;

		v.push_back(LIBCXX_NAMESPACE::ref<valueObj>::create());

		auto mcguffin=m->insert(std::make_pair(0, v.front()));

		std::cout << "test4u: this should be 0: "
			  << m->insert(std::make_pair(0, v.front())).null()
			  << std::endl;

		std::cout << "test4u: this should be 0: "
			  << m->insert(std::make_pair(1, v.front())).null()
			  << std::endl;
		v.clear();

		for (const auto &v:*m)
		{
			std::cout << "test4u: remaining key " << v.first
				  << std::flush
				  << " value: " << v.second.getptr()->v_proof
				  << std::endl;
		}
	}

	std::vector<LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>> save_mcguffin;

	{
		auto v=LIBCXX_NAMESPACE::ref<valueObj>::create();

		auto mcguffin1=m->find_or_create(0, [&v]
						 {
							 return v;
						 });

		auto mcguffin2=m->find_or_create(0, [&v]
						 {
							 return v;
						 });

		for (const auto &v:*m)
		{
			std::cout << "test4u: after find_or_create: " << v.first
				  << std::flush
				  << " value: " << v.second.getptr()->v_proof
				  << std::endl;

			auto p=v.second.mcguffin();

			std::cout << "test4u: mcguffin.null() should be 0: "
				  << p.null() << std::endl;
			save_mcguffin.push_back(p);
		}
	}

	for (const auto &v:*m)
	{
		std::cout << "test4u: mcguffin should exist: " << v.first
			  << std::flush
			  << " value: " << v.second.getptr()->v_proof
				  << std::endl;

		save_mcguffin.clear();

		std::cout << "test4u: mcguffin should work: " << v.first
			  << std::flush
			  << " null: " << v.second.getptr().null()
			  << std::endl;
	}
}

static void test5()
{
	auto m=LIBCXX_NAMESPACE::mcguffinmap
		<int, LIBCXX_NAMESPACE::ref<valueObj>>::create();

	auto v=LIBCXX_NAMESPACE::ref<valueObj>::create();

	std::vector<LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>> mcguffin;

	mcguffin.push_back(m->insert(0, v));

	auto b=m->begin();

	std::cout << "test5: null: " << b->second.getptr().null() << std::endl;
	mcguffin.clear();
	std::cout << "test5: null: " << b->second.getptr().null() << std::endl;
	std::cout << "test5: size: " << m->size() << std::endl;

	auto ret=m->find_or_create(0, [&v]
				   {
					   return v;
				   });

	std::cout << "test5: nulls: " << ret.first.null()
		  << ", " << ret.second.null() << std::endl;
}

int main(int argc, char **argv)
{
	try {
		alarm(60);

		test1();
		test2();
		test3();
		test3u();
		test4();
		test4u();
		test5();
	} catch (LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
	}
	return 0;
}
