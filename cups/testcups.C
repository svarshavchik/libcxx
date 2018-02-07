/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/cups/available.H"
#include "x/cups/destination.H"
#include "x/cups/job.H"
#include "x/exception.H"
#include "x/locale.H"
#include "x/visitor.H"
#include <iostream>
#include <algorithm>
#include <optional>

#include "testcups.h"
#include "../base/gettext_in.h"

using namespace LIBCXX_NAMESPACE;

void foobar()
{
}

void list()
{
	auto dests=cups::available_destinations();

	bool first=true;

	for (const auto &d:dests)
	{
		if (!first)
			std::cout << std::endl;
		first=false;
		std::cout << d->name();

		const char *sep=" (";
		const char *suf="";
		static const char close_paren[]=")";

		if (d->is_default())
		{
			std::cout << sep << "default";
			sep=", ";
			suf=close_paren;
		}

		if (d->is_discovered())
		{
			std::cout << sep << "discovered";
			suf=close_paren;
		}

		std::cout << suf << std::endl;

		auto options=d->options();

		std::vector<std::string> sorted_options;

		sorted_options.reserve(options.size());

		for (const auto &o:options)
			sorted_options.push_back(o.first + "=" + o.second);

		std::sort(sorted_options.begin(), sorted_options.end());

		for (const auto &o:sorted_options)
			std::cout << "    " << o << std::endl;
	}
}

bool dump_values(std::ostream &o,
		 const char *sep,
		 const cups::option_values_t &values)
{
	std::vector<std::string> sorted_option_values;

	if (!std::visit(visitor {
			[](const std::monostate) { return false; },

				// Shouldn't get:
			[&](const std::unordered_map<std::string,
			    std::u32string> &m) { return false; },

				// Shouldn't get:
			[&](const std::unordered_map<int, std::u32string> &m)
			{ return false; },

			[&](const std::unordered_map<std::string,
			    std::string> &m)
			{
				for (const auto &v:m)
				{
					auto n=v.first;

					if (!v.second.empty())
						n = n + " ("
							+ v.second
							+ ")";

					sorted_option_values
						.push_back(n);
				}

				std::sort(sorted_option_values.begin(),
					  sorted_option_values.end());
				return true;
			},
			[&](const std::unordered_map<int, std::string> &s)
			{
				for (const auto &v:s)
				{
					std::ostringstream o;

					o << v.second << " ("
					  << std::to_string(v.first) << ")";

					sorted_option_values
						.push_back(o.str());
				}

				std::sort(sorted_option_values.begin(),
					  sorted_option_values.end());
				return true;
			},
			[&](const std::unordered_set<int> &s)
			{
				std::vector<int> sorted_s{s.begin(),
						s.end()};

				std::sort(sorted_s.begin(),
					  sorted_s.end());

				for (const auto &v:sorted_s)
				{
					sorted_option_values.push_back
						(std::to_string(v));
				}
				return true;
			},
			[&](const std::vector<std::tuple<int,int>> &s)
			{
				for (const auto &v:s)
				{
					auto &[lower,upper]=v;

					std::ostringstream o;

					o << std::to_string(lower)
					  << "-" << std::to_string(upper);

					sorted_option_values.push_back
						(o.str());
				}
				return true;
			},
			[&](const std::unordered_set<bool> &s)
			{
				if (s.find(false) != s.end())
					sorted_option_values.push_back
						("false");
				if (s.find(true) != s.end())
					sorted_option_values.push_back
						("true");
				return true;
			},
			[&](const std::vector<cups::resolution> &v)
			{
				for (const auto &r:v)
					sorted_option_values.push_back(r);

				return true;
			},
			[&](const std::vector<cups::const_collection> &cv)
			{
				sorted_option_values.reserve(cv.size());

				foobar();

				for (const auto &c:cv)
				{
					std::vector<std::string> values;

					values.reserve(c->size());

					for (const auto &value:*c)
					{
						std::ostringstream o;

						o << value.first;

						dump_values(o, "=",
							    value.second);
						values.push_back(o.str());
					}
					std::sort(values.begin(),
						  values.end());

					std::ostringstream o;

					o << "[";
					const char *sep="";

					for (const auto &s:values)
					{
						o << sep << s;
						sep=", ";
					}
					o << "]";
					sorted_option_values.push_back(o.str());
				}
				return true;
			}}, values))
		return false;

	if (sorted_option_values.empty())
		return false;

	for (const auto &v:sorted_option_values)
	{
		o << sep << v;
		sep=", ";
	}
	return true;
}

int info(const std::optional<std::string> &printer)
{
	auto dests=cups::available_destinations();

	auto iter=std::find_if
		(dests.begin(), dests.end(),
		 [&]
		 (const auto &v)
		 {
			 return printer ? v->name() == *printer:v->is_default();
		 });

	if (iter == dests.end())
	{
		std::cerr << "Printer not found." << std::endl;
		return 1;
	}

	auto info=(*iter)->info();

	auto options=info->supported_options();

	std::vector<std::string> sorted_options{options.begin(), options.end()};

	std::sort(sorted_options.begin(), sorted_options.end());
	for (const auto &option:sorted_options)
	{
		std::cout << option;

		info->supported(option);
		auto values=info->option_values(option);

		dump_values(std::cout, ": ", values);
		std::cout << std::endl;

		values=info->ready_option_values(option);
		if (dump_values(std::cout, "    (ready: ", values))
			std::cout << ")" << std::endl;

		values=info->default_option_values(option);
		if (dump_values(std::cout, "    (default: ", values))
			std::cout << ")" << std::endl;
	}

	auto user_defaults=info->user_defaults();

	if (!user_defaults.empty())
	{
		std::cout << std::endl
			  << "User defaults:" << std::endl;

		sorted_options.clear();
		sorted_options.reserve(user_defaults.size());

		for (const auto &d:user_defaults)
			sorted_options.push_back(d.first + "=" + d.second);

		std::sort(sorted_options.begin(), sorted_options.end());
		for (const auto &option:sorted_options)
		{
			std::cout << "    " << option << std::endl;
		}
	}

	return 0;
}

void print(const std::optional<std::string> &printer,
	   const std::list<std::string> &files)
{
	if (files.empty())
		return;

	auto dests=cups::available_destinations();

	auto iter=std::find_if
		(dests.begin(), dests.end(),
		 [&]
		 (const auto &v)
		 {
			 return printer ? v->name() == *printer:v->is_default();
		 });

	if (iter == dests.end())
		throw EXCEPTION("Printer not found.");

	auto job=(*iter)->info()->create_job();

	for (const auto &f:files)
		job->add_document_file(f, f);

	std::cout << "Created print job " << job->submit("testcups")
		  << std::endl;
}

int main(int argc, char *argv[])
{
	try {
		auto l=locale::base::environment();

		l->global();

		testcups options{messages::create(l, LIBCXX_DOMAIN)};

		auto args=options.parse(argc, argv)->args;

		if (options.info->value)
		{
			std::optional<std::string> printer;

			if (!args.empty())
				printer=*args.begin();

			return (info(printer));
		}

		if (options.print->value)
		{
			std::optional<std::string> printer;

			if (options.printer->isSet())
				printer=options.printer->value;

			print(printer, args);
			return 0;
		}

		list();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
	}
	return 0;
}
