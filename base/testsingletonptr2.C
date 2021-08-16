/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "testsingletonptr.H"


int testsingletonptr()
{
	typedef LIBCXX_NAMESPACE::singletonptr<testSingletonPtrObj> global;

	int x=0;

	{
		global glob{LIBCXX_NAMESPACE::ref<testSingletonPtrObj>::create()};

		if (!glob)
			return x;

		global glob2;

		if (glob2)
			x=glob2->x;
	}

	global glob3;

	if (!glob3)
		return x;

	return 0;
}
