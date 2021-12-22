/*
** Copyright 2015-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <string>
#include "x/ref.H"
#include "x/exception.H"
#include "x/appid.H"
#include "x/config.H"
#include <x/obj.H>
#include <iostream>

using namespace LIBCXX_NAMESPACE;

class D;

class A : virtual public obj {

public:

	virtual D *getd() { return nullptr; }

};

class B : public A {
};

class D : public B {

public:

	int counter=0;

	static D *cast_from(A *a)
	{
		return a->getd();
	}

	D *getd() override { ++counter; return this; }
};

void testconvert()
{
	ref<A> a=ref<D>::create();

	ref<B> b=a;

	a=b;

	ref<D> d(a);

	if (d->counter != 1)
		throw EXCEPTION("Didn't use getd() for some reason.");
}

int main()
{
	try {
		std::cout << "appid: " << appid() << std::endl;
		std::cout << "appver: " << appver() << std::endl;
		testconvert();
	} catch(const exception &e)
	{
		std::cout << e << std::endl;
		exit(1);
	}
	return (0);
}
