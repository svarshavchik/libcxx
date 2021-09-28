/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "job_impl.H"
#include <algorithm>
#include "x/sentry.H"
#include "x/visitor.H"
#include "x/join.H"
#include "x/strtok.H"
#include "x/cups/collection.H"
#include "x/cups/destination.H"
#include <cstring>

namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

job_implObj::job_implObj(const ref<destination_implObj> &destination)
	: destination{destination}
{
}

void job_implObj::in_thread(const functionref<void (cups_dest_t *,
						    http_t *,
						    cups_dinfo_t *,
						    job_info_s &)> &closure)
{
	destination->in_thread([&]
			       (auto dest,
				auto http,
				auto info)
	{
		closure(dest, http, info, job_info_in_thread);
	});
}

job_implObj::~job_implObj()
{
	in_thread([]
		  (auto dest,
		   auto http,
		   auto info,
		   auto &job_info)
	{
		cupsFreeOptions(job_info.num_options, job_info.options);
	});
}

void job_implObj::set_option(const std::string_view &name,
			     const std::string_view &value)
{
	size_t s=name.size();

	char name_str[s+1];

	std::copy(name.begin(), name.end(), name_str);
	name_str[s]=0;

	s=value.size();
	char value_str[s+1];

	std::copy(value.begin(), value.end(), value_str);
	value_str[s]=0;

	in_thread([name=&*name_str,
		   value=&*value_str]
		  (auto dest,
		   auto http,
		   auto info,
		   auto &job_info)
	{
		job_info.num_options=cupsAddOption(name, value,
						   job_info.num_options,
						   &job_info.options);
	});
}

void job_implObj::set_option(const std::string_view &name,
			     int value)
{
	size_t s=name.size();

	char name_str[s+1];

	std::copy(name.begin(), name.end(), name_str);
	name_str[s]=0;

	in_thread([&, name=&*name_str]
		  (auto dest,
		   auto http,
		   auto info,
		   auto &job_info)
	{

		job_info.num_options=cupsAddIntegerOption(name, value,
							  job_info.num_options,
							  &job_info.options);
	});
}


static std::string serialize_collection(const const_collection &c);

std::string range_to_string(const std::vector<std::tuple<int, int>> &arg)
{
	std::vector<std::string> values;

	values.reserve(arg.size());
	for (const auto &r:arg)
	{
		const auto &from=std::get<0>(r);
		const auto &to=std::get<1>(r);

		values.push_back(std::to_string(from)
				 + (from == to ? ""
				    : "-" + std::to_string(to)));
	}

	return join(values, ",");
}

static std::string serialize_values(const option_values_t &option_values)
{
	std::vector<std::string> values;

	std::visit(visitor{
			[&](const std::monostate &arg)
			{
				values.push_back("true");
			},
		        [&](const std::unordered_map<std::string,
			    std::string> &arg)
			{
				values.reserve(arg.size());

				for (const auto &v:arg)
					values.push_back(v.first);
			},
		        [&](const std::unordered_map<std::string,
			    std::u32string> &arg)
			{
				values.reserve(arg.size());

				for (const auto &v:arg)
					values.push_back(v.first);
			},
		        [&](const std::unordered_map<int, std::string> &arg)
			{
				values.reserve(arg.size());

				for (const auto &v:arg)
					values.push_back(std::to_string
							 (v.first));
			},
		        [&](const std::unordered_map<int, std::u32string> &arg)
			{
				values.reserve(arg.size());

				for (const auto &v:arg)
					values.push_back(std::to_string
							 (v.first));
			},
		        [&](const std::unordered_set<int> &arg)
			{
				values.reserve(arg.size());

				for (const auto &v:arg)
					values.push_back(std::to_string(v));
			},
		        [&](const std::unordered_set<bool> &arg)
			{
				values.reserve(arg.size());

				for (const auto &v:arg)
					values.push_back(v? "true":"false");
			},
		        [&](const std::vector<resolution> &arg)
			{
				values.reserve(arg.size());

				for (const auto &r:arg)
				{
					values.push_back(std::to_string
							 (r.xres) + "x" +
							 std::to_string
							 (r.yres) +
							 (r.units ==
							  resolution::per_inch
							  ? "dpi":"dpcm"));
				}
			},
		        [&](const std::vector<std::tuple<int, int>> &arg)
			{
				values.push_back(range_to_string(arg));
			},
			[&](const std::vector<const_collection> &arg)
			{
				values.reserve(arg.size());
				for (const auto &r:arg)
				{
					values.push_back
						(std::string{"{"} +
						 serialize_collection(r) +
						 std::string{"}"});
				}
			}

		}, option_values);

	return join(values, ",");
}

static std::string serialize_collection(const const_collection &c)
{
	std::ostringstream o;
	const char *sep="";

#ifndef DEBUG_FOR_SERIALIZE_COLLECTION
#define DEBUG_FOR_SERIALIZE_COLLECTION const auto &value:*c
#endif

	for (DEBUG_FOR_SERIALIZE_COLLECTION)
	{

		o << sep << value.first << "=";
		auto v=serialize_values(value.second);

		// STRINGCHAR    = %x5C %x20 / %x5C %x5C / %x5C DQUOTE / %x5C SQUOTE /
                //                %x5C 3OCTALDIGIT / %x21 / %x23-26 / %x28-5B /
		//                %x5D-7E / %xA0-FF

		for (unsigned char c:v)
		{
			if (c == 0x21 || (c >= 0x23 && c <= 0x26) ||
			    (c >= 0x28 && c <= 0x5B) ||
			    (c >= 0x5D && c <= 0x7E))
			{
				o << (char)c;
				continue;
			}
			o << (char)0x5c;

			switch (c) {
			case ' ':
			case 0x5c:
			case '"':
			case '\'':
				o << (char)c;
				break;
			default:
				o << (c / 64 + '0')
				  << (((c / 8) % 8 + '0'))
				  << ((c % 8) + '0');
				break;
			}
		}
		sep=" ";
	}
	return o.str();
}

std::optional<std::vector<std::tuple<int, int>
			  >> parse_range_string(const std::string_view &s)
{
	std::vector<std::tuple<int, int>> r;

	std::vector<std::string> ranges;

	strtok_str(s, ",", ranges);

	r.reserve(ranges.size());

	for (const auto &range:ranges)
	{
		std::vector<std::string> from_to;
		std::vector<int> from_to_int;

		from_to.reserve(2);
		from_to_int.reserve(2);

		strtok_str(range, " \t\r\n-", from_to);

		if (from_to.empty())
			continue;

		if (from_to.size() > 2)
			return {};

		for (const auto &s:from_to)
		{
			int n=0;

			for (char c:s)
			{
				if (c < '0' || c > '9')
					return {};
				n=n*10+c-'0';
			}
			from_to_int.push_back(n);
		}

		auto b=from_to_int.begin();

		if (from_to_int.size() == 1)
			from_to_int.push_back(*b);

		// We reserve()d 2, guarantee no reallocation occurs.

		if (b[1] < b[0])
			return {};

		if (!r.empty())
		{
			if (std::get<1>(*--r.end()) > b[0])
				return {};
		}

		r.emplace_back(b[0], b[1]);
	}
	return r;
}

void job_implObj::set_option(const std::string_view &name,
			     const option_values_t &values)
{
	set_option(name, serialize_values(values));
}

void job_implObj::add_document(const std::string &name,
			       const document_t &document)
{
	in_thread([&]
		  (auto dest,
		   auto http,
		   auto info,
		   auto &job_info)
	{
		job_info.documents.push_back({name, document});
	});
}

int job_implObj::submit(const std::string_view &title)
{
	int result;

	in_thread([&]
		  (auto dest,
		   auto http,
		   auto info,
		   auto &job_info)
	{
		result=submit(title, dest, http, info, job_info);
	});

	return result;
}

int job_implObj::submit(const std::string_view &title,
			cups_dest_t *dest,
			http_t *http,
			cups_dinfo_t *info,
			job_info_s &job_info)
{
	size_t s=title.size();

	char title_str[s+1];

	std::copy(title.begin(), title.end(), title_str);
	title_str[s]=0;

	if (job_info.documents.empty())
		return 0;

	int job_id;

	auto status=cupsCreateDestJob(http,
				      dest,
				      info,
				      &job_id,
				      title_str,
				      job_info.num_options,
				      job_info.options);

	if (status != IPP_STATUS_OK)
		throw EXCEPTION(ippErrorString(status));

	auto job_sentry=make_sentry([&]
				    {
					    cupsCancelDestJob(http,
							      dest,
							      job_id);
				    });

	job_sentry.guard();

	for (const auto &doc:job_info.documents)
	{
		auto [mime_type, contents]=doc.document();

		auto status=cupsStartDestDocument(http,
						  dest, info,
						  job_id,
						  doc.name.c_str(),
						  mime_type.c_str(),
						  0, NULL, 0);
		if (status != HTTP_STATUS_CONTINUE)
			throw EXCEPTION(httpStatus(status));

		auto doc_sentry=make_sentry
			([&]
			 {
				 cupsFinishDestDocument(http,
							dest,
							info);
			 });
		doc_sentry.guard();

		while (auto res=contents())
		{
			status=cupsWriteRequestData(http,
						    res->data(),
						    res->size());

			if (status != HTTP_STATUS_CONTINUE)
				throw EXCEPTION(httpStatus(status));
		}
		doc_sentry.unguard();

		auto ipp_status=cupsFinishDestDocument(http,
						       dest,
						       info);

		if (ipp_status != IPP_STATUS_OK)
			throw EXCEPTION(cupsLastErrorString());
	}

	job_sentry.unguard();

	auto ipp_status=cupsCloseDestJob(http,
					 dest,
					 info,
					 job_id);

	if (ipp_status != IPP_STATUS_OK)
		throw EXCEPTION(cupsLastErrorString());
	return job_id;
}

#if 0
{
#endif
}
