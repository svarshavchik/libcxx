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
	std::list<testvip> initial, update;

	testviphandler() {}
	~testviphandler() noexcept {}

	void cbinitial(const testvip &n)
	{
		initial.push_back(n);
	}

	void cbupdate(const testvip &u)
	{
		update.push_back(u);
	}

};

class invoke_initial {

public:
	static void event(const LIBCXX_NAMESPACE::ptr<testviphandler> &h,
			  const testvip &v)
	{
		h->cbinitial(v);
	}
};

class invoke_update {

public:
	static void event(const LIBCXX_NAMESPACE::ptr<testviphandler> &h,
			  const testvip &v)
	{
		h->cbupdate(v);
	}
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

	auto h=LIBCXX_NAMESPACE::ref<testviphandler>::create();
	auto i=LIBCXX_NAMESPACE::ref<testviphandler>::create();

	{
		handlerlock l(vip);

		l.install(h, *readlock(vip));
	}

	if (h->initial.size() != 1 ||
	    h->update.size() != 0 ||
	    h->initial.front().n != 2)
		throw EXCEPTION("test4 failed (2)");

	{
		updatelock l(vip);

		l.update(testvip(4));
	}

	if (h->initial.size() != 1 ||
	    h->update.size() != 1 ||
	    h->update.front().n != 4)
		throw EXCEPTION("test4 failed (3)");

	{
		handlerlock l(vip);

		l.install(i, *readlock(vip));
	}

	if (h->initial.size() != 1 ||
	    h->update.size() != 1 ||
	    i->initial.size() != 1 ||
	    i->update.size() != 0 ||
	    i->initial.front().n != 4)
		throw EXCEPTION("test4 failed (4)");

	writelock(vip)->n=6;

	if (h->initial.size() != 1 ||
	    h->update.size() != 1 ||
	    i->initial.size() != 1 ||
	    i->update.size() != 0)
		throw EXCEPTION("test4 failed (5)");
}

void test4()
{
	test4_pass<LIBCXX_NAMESPACE::vipobj<testvip, testviphandler,
					  invoke_update, invoke_initial> >();

	test4_pass<LIBCXX_NAMESPACE::vipobjdebug<testvip, testviphandler,
					       invoke_update,
					       invoke_initial> >();

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


int main(int argc, char **argv)
{
	try {
		alarm(30);
		test1();
		test2();
		test3();
		test4();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
