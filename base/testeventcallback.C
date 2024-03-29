/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/vipobj.H"
#include <iostream>
#include <cstdlib>
#include <unistd.h>

class testvip {

public:
	int n;

	testvip(int nValue) : n(nValue) {}
};

class testviphandler : virtual public LIBCXX_NAMESPACE::obj {

public:
	std::list<testvip> seen;

	testviphandler() {}
	~testviphandler() {}
};

template<typename vip_t>
auto lock_and_return(vip_t &vip,
		     const LIBCXX_NAMESPACE::ref<testviphandler> &seen)
{
	typedef typename vip_t::handlerlock handlerlock;
	typedef typename vip_t::readlock readlock;

	handlerlock l(vip);

	return l.install_front([=]
			       (const testvip &v)
			       {
				       seen->seen.push_back(v.n);
			       } , *readlock(vip));
}

template<typename vip_t> void test4_pass()
{
	vip_t vip(2);

	typedef typename vip_t::readlock readlock;
	typedef typename vip_t::writelock writelock;
	typedef typename vip_t::updatelock updatelock;
	typedef typename vip_t::handlerlock handlerlock;

	handlerlock *p=nullptr;

	if (p)
		;
	if (readlock(vip)->n != 2)
		throw EXCEPTION("test4 failed (1)");

	auto seen=LIBCXX_NAMESPACE::ref<testviphandler>::create();
	auto seen2=LIBCXX_NAMESPACE::ref<testviphandler>::create();

	auto cb1=lock_and_return(vip, seen);

	if (seen->seen.size() != 1 ||
	    seen->seen.front().n != 2)
		throw EXCEPTION("test4 failed (2)");

	{
		updatelock l(vip);

		l.update(testvip(4));
	}

	if (seen->seen.size() != 2 ||
	    seen->seen.back().n != 4)
		throw EXCEPTION("test4 failed (3)");

	auto cb2=lock_and_return(vip, seen2);

	if (seen->seen.size() != 2 ||
	    seen2->seen.size() != 1 ||
	    seen2->seen.front().n != 4)
		throw EXCEPTION("test4 failed (4)");

	writelock(vip)->n=6;

	if (seen->seen.size() != 2 ||
	    seen2->seen.size() != 1 ||
	    seen2->seen.front().n != 4)
		throw EXCEPTION("test4 failed (5)");
}

void test4()
{
	test4_pass<LIBCXX_NAMESPACE::vipobj<testvip> >();
}

void test5()
{
	typedef LIBCXX_NAMESPACE::vipobj<int> vip_t;

	vip_t v(2);

	int n=0;

	vip_t::readlock l(v);

	vip_t::handlerlock h(v);

	auto lambda=[&n]
		(int value)
		{
			n+=value;
		};

	h.install_front(lambda, *l);
	h.install_front(lambda);

	if (n != 2)
		throw EXCEPTION("test5 failed");
}

void test6()
{
	typedef LIBCXX_NAMESPACE::vipobj<int> vip_t;

	vip_t v(2);

	auto mcguffin=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>::create();

	int latest=0;
	int latest2=0;

	{
		vip_t::handlerlock h(v);

		h.attach_front(mcguffin,
			       [&latest]
			       (const LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE
				::obj> &mcguffin,
				int value)
			       {
				       latest=value;
			       },
			       *vip_t::readlock(v));

		h.attach_back(mcguffin,
			       [&latest2]
			       (const LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE
				::obj> &mcguffin,
				int value)
			       {
				       latest2=value;
			       },
			       *vip_t::readlock(v));
	}

	if (latest != 2 || latest2 != 2)
		throw EXCEPTION("test6 failed");

	{
		vip_t::updatelock l(v);

		l.update(4);
	}

	if (latest != 4 || latest2 != 4)
		throw EXCEPTION("test6 failed");

	mcguffin=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj>::create();

	{
		vip_t::updatelock l(v);

		l.update(5);
	}

	if (latest != 4 || latest2 != 4)
		throw EXCEPTION("test6 failed");
}

int main(int argc, char **argv)
{
	try {
		alarm(30);
		test4();
		test5();
		test6();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
