/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "testsingletonptr.H"
#include <iostream>
#include <cstdlib>

extern int testsingletonptr();

int main()
{
	if (testsingletonptr() != 4)
	{
		std::cerr << "singletonptr test failed." << std::endl;
		exit(1);
	}
	return 0;
}
