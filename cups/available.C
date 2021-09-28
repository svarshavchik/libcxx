/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/cups/available.H"
#include "x/chrcasecmp.H"

#include "available_dests.H"
#include "available_impl.H"

#include <utility>
#include <algorithm>
#include <tuple>

namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

availableObj::availableObj()
{
}

availableObj::~availableObj()=default;

//////////////////////////////////////////////////////////////////////////////

std::vector<available> available_destinations()
{
	auto cd=ref<available_destsObj>::create();

	std::vector<std::tuple<ref<available_implObj>, const char *>> v;

	cd->thr->in_thread
		([&]
		 (auto dests,
		  auto n_dests)
		{
			v.reserve(n_dests);

			auto b=dests;
			auto e=dests+n_dests;

			while (b != e)
			{
				v.emplace_back(ref<available_implObj>
					       ::create(cd, b),
					       b->name);
				++b;
			}

			std::sort(v.begin(), v.end(),
				  []
				  (const auto &a, const auto &b)
				  {
					  const auto &[ar, aname]=a;
					  const auto &[br, bname]=b;
					  return chrcasecmp::compare(aname,
								     bname)<0;
				  });
		});

	std::vector<available> ret;

	ret.reserve(v.size());
	for (const auto &t:v)
	{
		ret.push_back(std::get<0>(t));
	}

	return ret;
}

#if 0
{
#endif
}
