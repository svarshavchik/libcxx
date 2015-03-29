/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/chrcasecmp.H"

namespace LIBCXX_NAMESPACE {
	namespace chrcasecmp {
#if 0
	};
};
#endif

int compare(const std::string &a, const std::string &b) noexcept
{
	std::string::const_iterator ab(a.begin()), ae(a.end()),
		bb(b.begin()), be(b.end());

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
	{
#endif
	}
}
