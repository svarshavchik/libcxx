/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "destination_impl.H"
#include "job_impl.H"
#include "x/messages.H"
#include "x/exception.H"
#include "x/property_value.H"
#include "../base/gettext_in.h"
#include <string_view>
#include <string>
#include <sstream>
#include <courier-unicode.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstring>
#include <cstdint>
namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

static property::value<int> connect_timeout(LIBCXX_NAMESPACE_STR
					    "::cups::connect_timeout_ms",
					    30000);

destination_implObj::info_t::lock::lock(const destination_implObj &me)
	: available_destsObj::dests_t::lock{me.available_destinations->dests},
	mpobj<lock_info_s>::lock{me.info}
	{
	}

destination_implObj::info_t::lock::~lock()=default;

collectionObj::~collectionObj()=default;

destination_implObj::cups_http_t_wrapper::cups_http_t_wrapper(cups_dest_t *dest)
	: http{cupsConnectDest(dest, CUPS_DEST_FLAGS_NONE,
			       connect_timeout.get(),
			       NULL, NULL, 0, NULL, NULL)}
{
	if (!http)
		throw EXCEPTION(libmsg(_("Print queue connection failed.")));
}

destination_implObj::cups_http_t_wrapper::~cups_http_t_wrapper()
{
	httpClose(http);
}

destination_implObj::cups_dinfo_t_wrapper
::cups_dinfo_t_wrapper(http_t *http, cups_dest_t *dest)
	: info{cupsCopyDestInfo(http, dest)}
{
	if (!info)
		throw EXCEPTION(libmsg(_("Print destination not available.")));
}

destination_implObj::cups_dinfo_t_wrapper::~cups_dinfo_t_wrapper()
{
	cupsFreeDestInfo(info);
}

destination_implObj::lock_info_s::lock_info_s(cups_dest_t *dest)
	: dest{dest}, http{dest}, info{http, dest}
{
}

destination_implObj
::destination_implObj(const available_dests &available_destinations,
		      cups_dest_t *dest)
	: available_destinations{available_destinations},
	  info{dest}
{
}

destination_implObj::~destination_implObj()=default;

bool destination_implObj::supported(const std::string_view &option) const
{
	auto s=option.size();

	char option_s[s+1];

	std::copy(option.begin(), option.end(), option_s);
	option_s[s]=0;

	info_t::lock lock{*this};

	return !!cupsCheckDestSupported(lock->http,
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

	return !!cupsCheckDestSupported(lock->http,
					lock->dest,
					lock->info,
					option_s,
					value_s);
}

std::unordered_set<std::string> destination_implObj::supported_options() const
{
	std::unordered_set<std::string> s;

	info_t::lock lock{*this};

	auto attrs=cupsFindDestSupported(lock->http, lock->dest,
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

static std::tuple<const char *, bool> parse_unicode_option(const char *o)
{
	bool flag=false;

	if (strncmp(o, "{unicode}", 9) == 0)
	{
		o += 9;
		flag=true;
	}

	return {o, flag};
}

option_values_t
destination_implObj::option_values(const std::string_view &option)
	const
{
	auto s=option.size();

	char option_s[s+1];

	std::copy(option.begin(), option.end(), option_s);
	option_s[s]=0;

	auto[name, unicode_flag]=parse_unicode_option(option_s);
	info_t::lock lock{*this};

	auto attrs=cupsFindDestSupported(lock->http, lock->dest,
					 lock->info,
					 name);

	return parse_attribute_values(lock, attrs, name, unicode_flag);
}

option_values_t
destination_implObj::default_option_values(const std::string_view &option)
	const
{
	auto s=option.size();

	char option_s[s+1];

	std::copy(option.begin(), option.end(), option_s);
	option_s[s]=0;

	auto[name, unicode_flag]=parse_unicode_option(option_s);

	info_t::lock lock{*this};

	auto attrs=cupsFindDestDefault(lock->http, lock->dest,
				       lock->info,
				       name);

	return parse_attribute_values(lock, attrs, name, unicode_flag);
}

option_values_t
destination_implObj::ready_option_values(const std::string_view &option)
	const
{
	auto s=option.size();

	char option_s[s+1];

	std::copy(option.begin(), option.end(), option_s);
	option_s[s]=0;

	auto[name, unicode_flag]=parse_unicode_option(option_s);
	info_t::lock lock{*this};

	auto attrs=cupsFindDestReady(lock->http, lock->dest,
				     lock->info,
				     name);

	return parse_attribute_values(lock, attrs, name, unicode_flag);
}

static constexpr int str_to_int(const char *p, int v)
{
	if (!*p)
		return v;

	return str_to_int(p+1, v * 10 + *p-'0');
}

static const char *enum_string(const char *n, int value)
{
	if (strcmp(n, CUPS_ORIENTATION) == 0)
		switch (value) {
		case str_to_int(CUPS_ORIENTATION_PORTRAIT, 0):
			return _("Portrait");
		case str_to_int(CUPS_ORIENTATION_LANDSCAPE, 0):
			return _("Landscape");
		case 5:
			return _("Reverse Portrait");
		case 6:
			return _("Reverse Landscape");
		}

	if (strcmp(n, CUPS_PRINT_QUALITY) == 0)
		switch (value) {
		case str_to_int(CUPS_PRINT_QUALITY_DRAFT, 0):
			return _("Draft");
		case str_to_int(CUPS_PRINT_QUALITY_NORMAL, 0):
			return _("Normal");
		case str_to_int(CUPS_PRINT_QUALITY_HIGH, 0):
			return _("High");
		}

	if (strcmp(n, CUPS_FINISHINGS) == 0)
		switch (value) {
		case str_to_int(CUPS_FINISHINGS_BIND, 0):
			return _("Bind");
		case str_to_int(CUPS_FINISHINGS_COVER, 0):
			return _("Cover");
		case str_to_int(CUPS_FINISHINGS_FOLD, 0):
			return _("Fold");
		case str_to_int(CUPS_FINISHINGS_NONE, 0):
			return _("None");
		case str_to_int(CUPS_FINISHINGS_PUNCH, 0):
			return _("Punch");
		case str_to_int(CUPS_FINISHINGS_STAPLE, 0):
			return _("Staple");
		case str_to_int(CUPS_FINISHINGS_TRIM, 0):
			return _("Trim");
		}

	return ippEnumString(n, value);
}

static std::string print_color_mode(const std::string_view &v)
{
	if (v == CUPS_PRINT_COLOR_MODE_AUTO)
		return _("Automatic");
	if (v == CUPS_PRINT_COLOR_MODE_MONOCHROME)
		return _("Monochrome");
	if (v == CUPS_PRINT_COLOR_MODE_COLOR)
		return _("Color");
	return {v.begin(), v.end()};
}

static std::string sides(const std::string_view &v)
{
	if (v == CUPS_SIDES_ONE_SIDED)
		return _("Off");
	if (v == CUPS_SIDES_TWO_SIDED_PORTRAIT)
		return _("Duplex (Long Edge)");
	if (v == CUPS_SIDES_TWO_SIDED_LANDSCAPE)
		return _("Duplex (Short Edge)");
	return {v.begin(), v.end()};
}

static std::string no_localizer(const std::string_view &v)
{
	return {v.begin(), v.end()};
}

option_values_t
destination_implObj::parse_attribute_values(info_t::lock &lock,
					    ipp_attribute_t *attrs,
					    const char *option_s,
					    bool unicode_flag)
{
	if (!attrs)
		return {};

	auto count=ippGetCount(attrs);

	auto tag_value=ippGetValueTag(attrs);

	switch (tag_value) {
	case IPP_TAG_NOVALUE:
		return {};
	case IPP_TAG_RANGE:
		{
			std::vector<std::tuple<int, int>> v;

			v.reserve(count);

			for (decltype (count) i=0; i<count; i++)
			{
				int upper;

				int lower=ippGetRange(attrs, i, &upper);

				v.emplace_back(lower, upper);
			}
			return v;
		}
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
		if (unicode_flag)
		{
			auto l=locale::base::global();

			std::unordered_map<int, std::u32string> v;

			for (decltype (count) i=0; i<count; i++)
			{
				auto value=ippGetInteger(attrs, i);

				auto s=enum_string(option_s, value);

				auto us=unicode::iconvert::tou
					::convert(s, l->charset()).first;

				v.emplace(value, us);
			}
			return v;
		}
		else
		{
			std::unordered_map<int, std::string> v;

			for (decltype (count) i=0; i<count; i++)
			{
				auto value=ippGetInteger(attrs, i);


				v.emplace(value, enum_string(option_s, value));
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
						   (lock, attr, n.c_str(),
						    unicode_flag));
				}
				v.push_back(c);
			}
			return v;
		}
	default:
		break;
	}

	std::unordered_map<std::string, std::string> v;

	bool is_media=strcmp(option_s, CUPS_MEDIA) == 0;

	auto localizer=strcmp(option_s, CUPS_SIDES) == 0
		? sides
		: strcmp(option_s, CUPS_PRINT_COLOR_MODE) == 0
		? print_color_mode
		: no_localizer;

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

		std::string lang_s;

		if (is_media)
		{
			cups_size_t size;

			lang_s=val;

			if (cupsGetDestMediaByName(lock->http,
						   lock->dest,
						   lock->info,
						   val,
						   CUPS_MEDIA_FLAGS_DEFAULT,
						   &size))
			{
				auto l=cupsLocalizeDestMedia
					(lock->http,
					 lock->dest,
					 lock->info,
					 CUPS_MEDIA_FLAGS_DEFAULT,
					 &size);

				if (l)
					lang_s=l;
			}
		}
		else
		{
			auto c=cupsLocalizeDestValue(lock->http,
						     lock->dest,
						     lock->info,
						     option_s,
						     val);

			if (c && strcmp(c, val))
			{
				lang_s=c;
			}
			else
			{
				lang_s=localizer(val);
			}
		}
		v.emplace(val, lang_s);
	}

	if (!unicode_flag)
		return v;

	auto l=locale::base::global();

	std::unordered_map<std::string, std::u32string> uv;

	for (const auto &s:v)
	{
		auto n=s.first;


		uv.emplace(s.first,
			   unicode::iconvert::tou::convert(s.second,
							   l->charset())
			   .first);
	}
	return uv;
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

job destination_implObj::create_job()
{
	return ref<job_implObj>::create(ref(this));
}


resolution::operator std::string() const
{
	std::ostringstream o;

	o << std::to_string(xres);

	if (xres != yres)
		o << "x" << std::to_string(yres);

	switch (units) {
	case cups::resolution::per_inch:
		o << "dpi";
		break;
	case cups::resolution::per_cm:
		o << "dpcm";
		break;
	default:
		break;
	}
	return o.str();
}

#if 0
{
#endif
}
