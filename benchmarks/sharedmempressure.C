/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <memory>
#include <vector>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <algorithm>

class xx;

class xx : public std::enable_shared_from_this<xx> {

public:
	int y[16];

	xx(int yArg) { y[0]=yArg; }
};

int main(int argc, char **argv)
{
	std::cout << std::ifstream("/proc/self/status").rdbuf();

	std::vector<std::shared_ptr<xx> > vec;

	for (size_t i=0; i<100000; ++i)
		vec.push_back(std::shared_ptr<xx>(new xx(4)));

	srand(1);

	for (size_t i=0; i<1000000; ++i)
	{
		size_t j=rand() % vec.size();

		if (vec[j].get() == nullptr)
			vec[j]=std::shared_ptr<xx>(new xx(4));
		else
			vec[j]=std::shared_ptr<xx>();
	}

	std::cout << std::ifstream("/proc/self/status").rdbuf();

	return (0);
}
