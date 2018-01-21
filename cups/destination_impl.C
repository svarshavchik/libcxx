/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "destination_impl.H"
#include "x/messages.H"
#include "x/exception.H"
#include "../base/gettext_in.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace LIBCXX_NAMESPACE {
	namespace cups {
#if 0
	}
};
#endif

destination_implObj::info_t::lock::lock(const destination_implObj &me)
	: available_destsObj::dests_t::lock{me.available_destinations->dests},
	mpobj<lock_info_s>::lock{me.info}
	{
	}

destination_implObj::info_t::lock::~lock()=default;

collectionObj::~collectionObj()=default;

destination_implObj
::destination_implObj(const available_dests &available_destinations,
		      cups_dest_t *dest)
	: available_destinations{available_destinations},
	  info{lock_info_s{dest, cupsCopyDestInfo(CUPS_HTTP_DEFAULT, dest)}}
{
	info_t::lock lock{*this};

	if (!lock->info)
		throw EXCEPTION(libmsg(_("Print destination not available.")));
}

destination_implObj::~destination_implObj()
{
	info_t::lock lock{*this};

	cupsFreeDestInfo(lock->info);
}

bool destination_implObj::supported(const std::string_view &option) const
{
	auto s=option.size();

	char option_s[s+1];

	std::copy(option.begin(), option.end(), option_s);
	option_s[s]=0;

	info_t::lock lock{*this};

	return !!cupsCheckDestSupported(CUPS_HTTP_DEFAULT,
					lock->dest,
					lock->info,
					option_s,
					nullptr);
}

bool destination_implObj::supported(const std::string_view &option,
				    const std::string_view &value) const
{
	auto s=option.size();

	char option_s[s+1];

	std::copy(option.begin(), option.end(), option_s);
	option_s[s]=0;

	s=value.size();

	char value_s[s+1];
	std::copy(value.begin(), value.end(), value_s);
	value_s[s]=0;

	info_t::lock lock{*this};

	return !!cupsCheckDestSupported(CUPS_HTTP_DEFAULT,
					lock->dest,
					lock->info,
					option_s,
					value_s);
}

std::unordered_set<std::string> destination_implObj::supported_options() const
{
	std::unordered_set<std::string> s;

	info_t::lock lock{*this};

	auto attrs=cupsFindDestSupported(CUPS_HTTP_DEFAULT, lock->dest,
					 lock->info,
					 "job-creation-attributes");

	if (!attrs)
		return s;

	auto count=ippGetCount(attrs);

	for (decltype (count) i=0; i<count; i++)
	{
		s.insert(ippGetString(attrs, i, nullptr));
	}
	return s;
}

option_values_t
destination_implObj::option_values(const std::string_view &option)
	const
{
	auto s=option.size();

	char option_s[s+1];

	std::copy(option.begin(), option.end(), option_s);
	option_s[s]=0;

	info_t::lock lock{*this};

	auto attrs=cupsFindDestSupported(CUPS_HTTP_DEFAULT, lock->dest,
					 lock->info,
					 option_s);

	return parse_attribute_values(attrs, option_s);
}

option_values_t
destination_implObj::default_option_values(const std::string_view &option)
	const
{
	auto s=option.size();

	char option_s[s+1];

	std::copy(option.begin(), option.end(), option_s);
	option_s[s]=0;

	info_t::lock lock{*this};

	auto attrs=cupsFindDestDefault(CUPS_HTTP_DEFAULT, lock->dest,
				       lock->info,
				       option_s);

	return parse_attribute_values(attrs, option_s);
}

option_values_t
destination_implObj::ready_option_values(const std::string_view &option)
	const
{
	auto s=option.size();

	char option_s[s+1];

	std::copy(option.begin(), option.end(), option_s);
	option_s[s]=0;

	info_t::lock lock{*this};

	auto attrs=cupsFindDestReady(CUPS_HTTP_DEFAULT, lock->dest,
				     lock->info,
				     option_s);

	return parse_attribute_values(attrs, option_s);
}

option_values_t
destination_implObj::parse_attribute_values(ipp_attribute_t *attrs,
					    const char *option_s)
{
	if (!attrs)
		return {};

	auto count=ippGetCount(attrs);

	auto tag_value=ippGetValueTag(attrs);

	switch (tag_value) {
	case IPP_TAG_NOVALUE:
		return {};
	case IPP_TAG_RESOLUTION:
		{
			std::vector<resolution> v;

			v.reserve(count);

			for (decltype (count) i=0; i<count; i++)
			{
				resolution res;
				ipp_res_t units;

				res.xres=ippGetResolution(attrs, i,
							  &res.yres, &units);

				switch (units) {
				case IPP_RES_PER_INCH:
					res.units=res.per_inch;
					break;
				case IPP_RES_PER_CM:
					res.units=res.per_cm;
					break;
				default:
					res.units=res.unknown;
				}
				v.push_back(res);
			}
			return v;
		}

	case IPP_TAG_INTEGER:
		{
			std::unordered_set<int> v;

			for (decltype (count) i=0; i<count; i++)
			{
				v.insert(ippGetInteger(attrs, i));
			}
			return v;
		}
	case IPP_TAG_BOOLEAN:
		{
			std::unordered_set<bool> v;

			for (decltype (count) i=0; i<count; i++)
			{
				v.insert(ippGetBoolean(attrs, i));
			}
			return v;
		}
	case IPP_TAG_ENUM:
		{
			std::unordered_map<int, std::string> v;

			for (decltype (count) i=0; i<count; i++)
			{
				auto value=ippGetInteger(attrs, i);

				v.emplace(value, ippEnumString(option_s,
							       value));
			}
			return v;
		}
	case IPP_TAG_BEGIN_COLLECTION:
		{
			std::vector<const_collection> v;

			v.reserve(count);

			for (decltype (count) i=0; i<count; i++)
			{
				auto ippc=ippGetCollection(attrs, i);

				auto c=collection::create();

				for (auto attr=ippFirstAttribute(ippc);
				     attr;
				     attr=ippNextAttribute(ippc))
				{
					std::string n=ippGetName(attr);
					c->emplace(n, parse_attribute_values
						   (attr, n.c_str()));
				}
				v.push_back(c);
			}
			return v;
		}
	default:
		break;
	}

	std::unordered_map<std::string, std::string> v;

	for (decltype (count) i=0; i<count; i++)
	{
		const char *lang=0;

		auto val=ippGetString(attrs, i, &lang);

		if (!val)
		{

			std::ostringstream o;

			o << "{unknown tag "
			  << ippTagString(ippGetValueTag(attrs)) << "}";
			v.emplace(o.str(), "");
			continue;
		}
		v.emplace(val, lang ? lang:"");
	}

	return v;
}

std::unordered_map<std::string, std::string>
destination_implObj::user_defaults() const
{
	std::unordered_map<std::string, std::string> ret;

	info_t::lock lock{*this};

	for (decltype(lock->dest->num_options) i=0;
	     i < lock->dest->num_options; ++i)
	{
		ret.emplace(lock->dest->options[i].name,
			    lock->dest->options[i].value);
	}

	return ret;
}

#if 0
{
	{
#endif
	};
};
