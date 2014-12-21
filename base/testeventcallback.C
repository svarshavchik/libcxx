/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "eventcallback.H"
#include "vipobj.H"
#include "vipobjdebug.H"
#include <iostream>
#include <cstdlib>

class mycbObj : public LIBCXX_NAMESPACE::eventcallback<int>::baseObj {

public:
	int sum;

	mycbObj() : sum(0)
	{
	}

	~mycbObj() noexcept
	{
	}

	void event(const int &n)
	{
		sum += n;
	}
};

typedef LIBCXX_NAMESPACE::ptr<mycbObj> mycb;

void test1()
{
	LIBCXX_NAMESPACE::eventcallback<int> cb;

	mycb a(mycb::create()), b(mycb::create());

	cb.install(a);
	cb.install(b);

	cb.event(3);

	if (a->sum != 3 || b->sum != 3)
		throw EXCEPTION("First event failed");

	b=mycb();

	cb.event(2);

	if (a->sum != 5)
		throw EXCEPTION("Second event failed");

	int n=0;

	auto lambda=cb.create_callback([&n]
				       (int a)
				       {
					       n += a;
				       });
	cb.install(lambda);

	cb.event(9);

	if (n != 9)
		throw EXCEPTION("install_callback failed");
}

class mycbvoidObj : public LIBCXX_NAMESPACE::eventcallback<void>::baseObj {

public:
	int counter;

	mycbvoidObj() : counter(0)
	{
	}

	~mycbvoidObj() noexcept
	{
	}

	void event()
	{
		++counter;
	}
};

typedef LIBCXX_NAMESPACE::ptr<mycbvoidObj> mycbvoid;

void test2()
{
	mycbvoid cb(mycbvoid::create());

	LIBCXX_NAMESPACE::eventcallback<void> cblist;

	cblist.install(cb);

	cblist.event();

	if (cb->counter != 1)
		throw EXCEPTION("test2 failed");

	int n=0;

	auto lambda=cblist.create_callback([&n]
					   {
						   ++n;
					   });
	cblist.install(lambda);
	cblist.event();

	if (n != 1)
		throw EXCEPTION("install_callback failed");
}

class customcbObj : virtual public LIBCXX_NAMESPACE::obj {

public:

	int sum;

	customcbObj() : sum(0)
	{
	}

	~customcbObj() noexcept
	{
	}

	void event_int(int n)
	{
		sum += n;
	}

	void event_void()
	{
		sum = -sum;
	}
};

typedef LIBCXX_NAMESPACE::ptr<customcbObj> customcb;

class invoke_int {

public:

	static void event(const LIBCXX_NAMESPACE::ptr<customcbObj> &cb,
			  int arg)
	{
		cb->event_int(arg);
	}
};

class invoke_void {

public:

	static void event(const LIBCXX_NAMESPACE::ptr<customcbObj> &cb)

	{
		cb->event_void();
	}
};

void test3()
{
	customcb cb(customcb::create());

	LIBCXX_NAMESPACE::eventcallback<int, customcbObj, invoke_int> cb_int;

	LIBCXX_NAMESPACE::eventcallback<void, customcbObj, invoke_void> cb_void;

	cb_int.install(cb);
	cb_void.install(cb);

	cb_int.event(4);
	cb_void.event();

	if (cb->sum != -4)
		throw(EXCEPTION("test3 failed"));
}

class testvip {

public:
	int n;

	testvip(int nValue) : n(nValue) {}
};

class testviphandler : virtual public LIBCXX_NAMESPACE::obj {

public:
	std::list<testvip> seen;

	testviphandler() {}
	~testviphandler() noexcept {}
};

template<typename vip_t> void test4_pass()
{
	vip_t vip(2);

	typedef typename vip_t::readlock readlock;
	typedef typename vip_t::writelock writelock;
	typedef typename vip_t::updatelock updatelock;
	typedef typename vip_t::handlerlock handlerlock;

	if (readlock(vip)->n != 2)
		throw EXCEPTION("test4 failed (1)");

	auto seen=LIBCXX_NAMESPACE::ref<testviphandler>::create();
	auto seen2=LIBCXX_NAMESPACE::ref<testviphandler>::create();

	auto cb1=({
			handlerlock l(vip);

			l.install_front([=]
					(const testvip &v)
					{
						seen->seen.push_back(v.n);
					} , *readlock(vip));
		});

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

	auto cb2=({
			handlerlock l(vip);

			l.install_front([=]
					(const testvip &v)
					{
						seen2->seen.push_back(v.n);
					} , *readlock(vip));
		});

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

	test4_pass<LIBCXX_NAMESPACE::vipobjdebug<testvip>>();

	std::cerr << "The following exception is expected:" << std::endl;

	try {
		typedef LIBCXX_NAMESPACE::vipobjdebug<std::string> pool_t;

		pool_t pool;

		pool_t::readlock rlock(pool);

		pool_t::handlerlock hlock(pool);

	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
	}		
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
			       [&latest]
			       (const LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE
				::obj> &mcguffin,
				int value)
			       {
				       latest=value;
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
		test1();
		test2();
		test3();
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
