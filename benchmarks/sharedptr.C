/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <memory>
#include <iostream>
#include <sys/time.h>
#include <iomanip>

class xx {

public:
	int y;

	xx(int yArg) : y(yArg) {}
};

int main(int argc, char **argv)
{
	struct timeval tv1, tv2;

	std::shared_ptr<xx> a(new xx(4)), b(new xx(5));

	size_t n;

	gettimeofday(&tv1, NULL);

	for (n=0; n<10000000; n++)
	{
		std::shared_ptr<xx> c=a;

		a=b;
		b=c;
	}
	gettimeofday(&tv2, NULL);

	tv2.tv_usec -= tv1.tv_usec;
	tv2.tv_sec -= tv1.tv_sec;

	if (tv2.tv_usec < 0)
	{
		tv2.tv_usec += 1000000;
		--tv2.tv_sec;
	}

	std::cout << tv2.tv_sec << "." << std::setw(6) << std::setfill('0')
		  << tv2.tv_usec << std::endl;
	return 0;
}
