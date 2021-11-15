/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include <iostream>
#include <unistd.h>

#include "x/weakptr.H"
#include "x/fd.H"
#include "x/mpweakptr.H"
#include "x/threads/run.H"

using namespace LIBCXX_NAMESPACE;

void testweak4()
{
	ptr<obj> obj1=ptr<obj>::create(), obj2=ptr<obj>::create();

	auto weak1=mpweakptr<ptr<obj>>::create(obj1),
		weak2=mpweakptr<ptr<obj>>::create(obj2);

	weak1->setptr(weak2);
	weak1->getptr();

	weak1->setptr(ref<obj>::create());

	weak1->getptr();
}

int main(int argc, char **argv)
{
	alarm(30);
	testweak4();
}
