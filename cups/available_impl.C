/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "available_impl.H"
#include "destination_impl.H"

#include <utility>
#include <algorithm>
#include <sstream>
#include <cstdint>

namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

available_implObj
::available_implObj(const available_dests &available_destinations,
		    cups_dest_t *dest)
	: available_destinations{available_destinations},
	  dest{dest}
{
}

available_implObj::~available_implObj()=default;

void available_implObj::in_thread(const functionref<void (cups_dest_t *)>
				 &closure) const
{
	available_destinations->thr->in_thread
		([&]
		 (auto, auto)
		{
			closure(dest);
		});
}

std::string available_implObj::name() const
{
	std::string name;

	in_thread([&]
		  (auto dest)
	{
		name=dest->name;
	});

	return name;
}

bool available_implObj::is_default() const
{
	bool flag;

	in_thread([&]
		  (auto dest)
	{
		flag=!!dest->is_default;
	});

	return flag;
}

std::unordered_map<std::string,std::string> available_implObj::options() const
{
	std::unordered_map<std::string, std::string> m;

	in_thread([&]
		  (auto dest)
	{
		for (decltype(dest->num_options) j=0;
		     j<dest->num_options; ++j)
		{
			m.emplace(dest->options[j].name,
				  dest->options[j].value);
		}
	});

	return m;
}

bool available_implObj::is_discovered() const
{
	auto options=this->options();

	auto iter=options.find("printer-type");

	if (iter != options.end())
	{
		uint32_t type;

		std::istringstream i{iter->second};

		if (i >> type)
		{
			if (type & CUPS_PRINTER_DISCOVERED)
			{
				return true;
			}
		}
	}
	return false;
}

const_destination available_implObj::info() const
{
	ptr<destination_implObj> p;

	in_thread([&]
		  (auto dest)
	{
		p=ref<destination_implObj>::create(available_destinations,
						   dest);
	});

	return p;
}

destination available_implObj::info()
{
	ptr<destination_implObj> p;

	in_thread([&]
		  (auto dest)
	{
		p=ref<destination_implObj>::create(available_destinations,
						   dest);
	});

	return p;
}

#if 0
{
#endif
}
