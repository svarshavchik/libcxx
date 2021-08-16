/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/chrcasecmp.H"

namespace LIBCXX_NAMESPACE::chrcasecmp {
#if 0
}
#endif

std::size_t hash::operator()(const std::string_view &s) const noexcept
{
	std::size_t hash = 5381;

	for (unsigned char c:s)
		hash = hash * 33 + tolower(c);

        return hash;
}

int compare(const std::string_view &a, const std::string_view &b) noexcept
{
	auto ab{a.begin()}, ae{a.end()},
	     bb{b.begin()}, be{b.end()};

	while (ab != ae || bb != be)
	{
		if (ab == ae)
			return -1;

		if (bb == be)
			return 1;
		int n= (int)(unsigned char)toupper(*ab) -
			(int)(unsigned char)toupper(*bb);

		if (n)
			return n;

		++ab;
		++bb;
	}

	return 0;
}

#if 0
{
#endif
}
