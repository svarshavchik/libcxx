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

	std::vector<std::shared_ptr<xx> > vec, vec2;

	for (size_t i=0; i<1000000; ++i)
		vec.push_back(std::shared_ptr<xx>(new xx(4)));

	vec2=vec;

	std::cout << std::ifstream("/proc/self/status").rdbuf();

	return (0);
}
