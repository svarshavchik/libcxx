/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/cups/available.H"
#include "x/chrcasecmp.H"

#include "available_dests.H"
#include "available_impl.H"

#include <utility>
#include <algorithm>

namespace LIBCXX_NAMESPACE {
	namespace cups {
#if 0
	}
};
#endif

availableObj::availableObj()
{
}

availableObj::~availableObj()=default;

//////////////////////////////////////////////////////////////////////////////

std::vector<available> available_destinations()
{
	auto cd=ref<available_destsObj>::create();

	std::vector<ref<available_implObj>> v;

	v.reserve(cd->n_dests);

	available_destsObj::dests_t::lock lock{cd->dests};

	for (std::remove_cv_t<decltype(cd->n_dests)> i=0; i<cd->n_dests; ++i)
	{
		auto d=ref<available_implObj>::create(cd, (*lock)+i);
		v.push_back(d);
	}

	std::sort(v.begin(), v.end(),
		  []
		  (const auto &a, const auto &b)
		  {
			  available_implObj::dest_t::lock la{*a};
			  available_implObj::dest_t::lock lb{*b};

			  return chrcasecmp::compare(la->dest->name,
						     lb->dest->name) < 0;
		  });

	return {v.begin(), v.end()};
}

#if 0
{
	{
#endif
	};
};
