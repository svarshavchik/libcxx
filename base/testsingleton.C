/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/obj.H"
#include "x/ptr.H"
#include "x/singleton.H"

#include <iostream>

class singletonObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	singletonObj() noexcept
	{
		std::cout << "Constructor" << std::endl;
	}

	~singletonObj() noexcept
	{
		std::cout << "Destructor" << std::endl;
	}
};

LIBCXX_NAMESPACE::singleton<singletonObj> singleton;

int main()
{
	std::cout << "In main" << std::endl;
	singleton.get();
	singleton.get();
	std::cout << "Leaving main" << std::endl;

	return (0);
}
