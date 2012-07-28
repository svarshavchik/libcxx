/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "x/ref.H"
#include "x/obj.H"
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <algorithm>

class xx : virtual public LIBX_NAMESPACE::obj {

public:
	int y[16];

	xx(int yArg) { y[0]=yArg;}
};

void foo(const LIBX_NAMESPACE::const_ref<xx> &a)
{
}

void bar(const LIBX_NAMESPACE::ref<xx> &a)
{
	foo(a);
}

int main(int argc, char **argv)
{
	std::cout << std::ifstream("/proc/self/status").rdbuf();

	std::vector<LIBX_NAMESPACE::ptr<xx> > vec;

	for (size_t i=0; i<100000; ++i)
		vec.push_back(LIBX_NAMESPACE::ptr<xx>::create(4));

	srand(1);

	for (size_t i=0; i<1000000; ++i)
	{
		size_t j=rand() % vec.size();

		if (vec[j].null())
			vec[j]=LIBX_NAMESPACE::ptr<xx>::create(4);
		else
			vec[j]=LIBX_NAMESPACE::ptr<xx>();
	}

	std::cout << std::ifstream("/proc/self/status").rdbuf();

	bar(vec[0]);
	return (0);
}
