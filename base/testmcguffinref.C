/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/exception.H"
#include "x/mcguffinref.H"
#include "x/refptr_traits.H"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

class intval : virtual public LIBCXX_NAMESPACE::obj {

public:
	int v;

	intval() : v(0) {}

	~intval() {}

};

typedef LIBCXX_NAMESPACE::ref<intval> intval_ref;

typedef LIBCXX_NAMESPACE::const_ref<intval> constintval_ref;

#include "refcollection.H"


class cb : public LIBCXX_NAMESPACE::mcguffinrefObj<LIBCXX_NAMESPACE::ptr<intval> > {

public:
	cb() {}
	~cb() {}

	void destroyed(const LIBCXX_NAMESPACE::ptr<intval> &s)

	{
		++s->v;
	}
};

void test1()
{
	LIBCXX_NAMESPACE::ptr<intval> v(LIBCXX_NAMESPACE::ptr<intval>::create());

	LIBCXX_NAMESPACE::ptr<cb> r(LIBCXX_NAMESPACE::ptr<cb>::create());

	LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>
		mcguffin(LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>::create());

	r->install(v, mcguffin);

	v->v=1;

	if (r->getptr().null())
		throw EXCEPTION("test 1 failed");

	mcguffin=LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj>();

	if (!r->getptr().null())
		throw EXCEPTION("test 2 failed");

	if (v->v != 2)
		throw EXCEPTION("test 3 failed");
}

void test2()
{
	mycollectionptr p;

	mycollection r{p};
}

int main(int argc, char **argv)
{
	try {
		alarm(30);
		test1();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return (0);
}
