/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/cups/available.H"
#include <iostream>
#include <algorithm>

using namespace LIBCXX_NAMESPACE;

void list()
{
	auto dests=cups::available_destinations();

	bool first=true;

	for (const auto &d:dests)
	{
		if (!first)
			std::cout << std::endl;
		first=false;
		std::cout << d->name();

		if (d->is_default())
			std::cout << " (default)";
		std::cout << std::endl;

		auto options=d->options();

		std::vector<std::string> sorted_options;

		sorted_options.reserve(options.size());

		for (const auto &o:options)
			sorted_options.push_back(o.first + "=" + o.second);

		std::sort(sorted_options.begin(), sorted_options.end());

		for (const auto &o:sorted_options)
			std::cout << "    " << o << std::endl;
	}
}

int main()
{
	list();
	return 0;
}
