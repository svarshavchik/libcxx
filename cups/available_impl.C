/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "available_impl.H"
#include "destination_impl.H"

#include <utility>
#include <algorithm>

namespace LIBCXX_NAMESPACE {
	namespace cups {
#if 0
	}
};
#endif

available_implObj::dest_t::lock::lock(const available_implObj &me)
	: available_destsObj::dests_t::lock{me.available_destinations->dests},
	mpobj<locked_dests_s>::lock{me.dest}
	{
	}

available_implObj::dest_t::lock::~lock()=default;

available_implObj
::available_implObj(const available_dests &available_destinations,
		    cups_dest_t *dest)
	: available_destinations{available_destinations},
	  dest{locked_dests_s{dest}}
{
}

available_implObj::~available_implObj()=default;

std::string available_implObj::name() const
{
	dest_t::lock lock{*this};

	return lock->dest->name;
}

bool available_implObj::is_default() const
{
	dest_t::lock lock{*this};

	return !!lock->dest->is_default;
}

std::unordered_map<std::string,std::string> available_implObj::options() const
{
	dest_t::lock lock{*this};

	std::unordered_map<std::string, std::string> m;

	for (decltype(lock->dest->num_options) j=0;
	     j<lock->dest->num_options; ++j)
	{
		m.emplace(lock->dest->options[j].name,
			  lock->dest->options[j].value);
	}

	return m;
}

const_destination available_implObj::info() const
{
	dest_t::lock lock{*this};

	return ref<destination_implObj>::create(available_destinations,
						lock->dest);
}

destination available_implObj::info()
{
	dest_t::lock lock{*this};

	return ref<destination_implObj>::create(available_destinations,
						lock->dest);
}

#if 0
{
	{
#endif
	};
};