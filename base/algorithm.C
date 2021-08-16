/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/algorithm.H"
#include "x/exception.H"
#include "gettext_in.h"
#include <utility>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

void do_sort_by(std::vector<size_t> &v,
		const function<void (size_t, size_t)> &f)
{
	const size_t n=v.size();

	for (size_t i=0; i<n; ++i)
	{
		size_t j;

		while ((j=v[i]) != i)
		{
			if (j >= n || v[j] == j)
				throw EXCEPTION(_("Invalid sort index."));
			std::swap(v[i], v[j]);
			f(i,j);
		}
	}
}
#if 0
{
#endif
}
