/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "available_dests.H"

namespace LIBCXX_NAMESPACE {
	namespace cups {
#if 0
	}
};
#endif

available_destsObj::available_destsObj()
	: n_dests{cupsGetDests2(CUPS_HTTP_DEFAULT,
				&*dests_t::lock{this->dests})}
{
}

available_destsObj::~available_destsObj()
{
	dests_t::lock lock{dests};

	cupsFreeDests(n_dests, *lock);
}

#if 0
{
	{
#endif
	};
};
