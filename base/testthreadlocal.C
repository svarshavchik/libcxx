/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/threads/weaklocal.H"
#include "x/threads/run.H"
#include "x/threads/singleton.H"
#include "x/ptr.H"
#include "x/obj.H"

#include <iostream>
#include <cstdlib>

class loc : virtual public LIBCXX_NAMESPACE::obj {

public:
	int n;

	loc(int nArg) : n(nArg) {}
	~loc() noexcept {}
};

class dummyThread : public LIBCXX_NAMESPACE::obj {

public:
	dummyThread() {}
	~dummyThread() noexcept {}

	LIBCXX_NAMESPACE::weakthreadlocalptr<loc> ll;

	void run()
	{
		auto l=LIBCXX_NAMESPACE::ptr<loc>::create(1);

		ll=LIBCXX_NAMESPACE::makeweakthreadlocal(l);

		l=LIBCXX_NAMESPACE::ptr<loc>();

		l=ll->getptr();

		if (l.null() || l->n != 1)
			throw EXCEPTION("Object did not survive");
	}
};
		
static void testthreadlocal()
{
	auto l=LIBCXX_NAMESPACE::ptr<loc>::create(2);

	LIBCXX_NAMESPACE::weakthreadlocalptr<loc>
		ll=LIBCXX_NAMESPACE::makeweakthreadlocal(l);

	l=LIBCXX_NAMESPACE::ptr<loc>();

	l=ll->getptr();

	if (l.null() || l->n != 2)
		throw EXCEPTION("test 1 failed");

	auto dt=LIBCXX_NAMESPACE::ptr<dummyThread>::create();

	LIBCXX_NAMESPACE::run(dt)->get();

	if (dt->ll.null())
		throw EXCEPTION("test 2 failed");

	if (!dt->ll->getptr().null())
		throw EXCEPTION("test 3 failed");
}

template<int n> class testst : virtual public LIBCXX_NAMESPACE::obj {

public:
	int v;

	testst() : v(0) {}
	~testst() noexcept {}

	int next() noexcept
	{
		return v += n;
	}
};

LIBCXX_NAMESPACE::threadsingleton< LIBCXX_NAMESPACE::ptr<testst<1> > > s1;
LIBCXX_NAMESPACE::threadsingleton< LIBCXX_NAMESPACE::ptr<testst<2> > > s2;

template<typename singleton_type>
void threadsingletontest(singleton_type &instance, int expected_increment)

{
	int n=0;

	for (size_t i=0; i<3; ++i)
		if (instance.get()->next() != (n += expected_increment))
			throw EXCEPTION("thread singleton test failed");
}

class testThreadSingletonThread : public LIBCXX_NAMESPACE::obj {

public:
	int cnt;

	bool errflag;

	testThreadSingletonThread(int cntArg)
		: cnt(cntArg), errflag(false)
	{
	}

	~testThreadSingletonThread() noexcept {}

	void run()

	{
		try {
			threadsingletontest(s1, 1);

			if (cnt & 1)
				threadsingletontest(s2, 2);

			if (cnt)
			{
				auto t=LIBCXX_NAMESPACE::ptr<testThreadSingletonThread>
					::create(cnt-1);

				LIBCXX_NAMESPACE::run(t)->wait();

				if (t->errflag)
					errflag=true;
			}

			if (!(cnt & 1))
				threadsingletontest(s2, 2);
		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			std::cerr << e << std::endl;
			errflag=true;
		}
	}
};


void testthreadsingleton()
{
	threadsingletontest(s1, 1);

	auto t=LIBCXX_NAMESPACE::ref<testThreadSingletonThread>
		::create(4);

	LIBCXX_NAMESPACE::run(t)->wait();

	if (t->errflag)
		exit(1);

	threadsingletontest(s2, 2);
}

int main(int argc, char **argv)
{
	testthreadlocal();
	testthreadsingleton();
	return (0);
}

