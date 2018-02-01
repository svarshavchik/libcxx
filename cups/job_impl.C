/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "job_impl.H"
#include <algorithm>
#include "x/sentry.H"

namespace LIBCXX_NAMESPACE {
	namespace cups {
#if 0
	}
};
#endif

job_implObj::job_implObj(const ref<destination_implObj> &destination)
	: destination{destination}
{
}

job_implObj::~job_implObj()
{
	job_info_t::lock lock{job_info};

	cupsFreeOptions(lock->num_options, lock->options);
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

	job_info_t::lock lock{job_info};

	lock->num_options=cupsAddOption(name_str, value_str,
					lock->num_options,
					&lock->options);
}

void job_implObj::set_option(const std::string_view &name,
			     int value)
{
	size_t s=name.size();

	char name_str[s+1];

	std::copy(name.begin(), name.end(), name_str);
	name_str[s]=0;

	job_info_t::lock lock{job_info};

	lock->num_options=cupsAddIntegerOption(name_str, value,
					       lock->num_options,
					       &lock->options);
}

void job_implObj::add_document(const std::string &name,
			       const document_t &document)
{
	job_info_t::lock lock{job_info};

	lock->documents.push_back({name, document});
}

int job_implObj::submit(const std::string_view &title)
{
	size_t s=title.size();

	char title_str[s+1];

	std::copy(title.begin(), title.end(), title_str);
	title_str[s]=0;

	job_info_t::lock job_lock{job_info};

	if (job_lock->documents.empty())
		return 0;

	destination_implObj::info_t::lock lock{*destination};

	int job_id;

	auto status=cupsCreateDestJob(lock->http,
				      lock->dest,
				      lock->info,
				      &job_id,
				      title_str,
				      job_lock->num_options,
				      job_lock->options);

	if (status != IPP_STATUS_OK)
		throw EXCEPTION(ippErrorString(status));

	auto job_sentry=make_sentry([&]
				    {
					    cupsCancelDestJob(lock->http,
							      lock->dest,
							      job_id);
				    });

	job_sentry.guard();

	for (const auto &doc:job_lock->documents)
	{
		auto [mime_type, contents]=doc.document();

		auto status=cupsStartDestDocument(lock->http,
						  lock->dest, lock->info,
						  job_id,
						  doc.name.c_str(),
						  mime_type.c_str(),
						  0, NULL, 0);
		if (status != HTTP_STATUS_CONTINUE)
			throw EXCEPTION(httpStatus(status));

		auto doc_sentry=make_sentry
			([&]
			 {
				 cupsFinishDestDocument(lock->http,
							lock->dest,
							lock->info);
			 });
		doc_sentry.guard();

		while (auto res=contents())
		{
			status=cupsWriteRequestData(lock->http,
						    res->data(),
						    res->size());

			if (status != HTTP_STATUS_CONTINUE)
				throw EXCEPTION(httpStatus(status));
		}
		doc_sentry.unguard();

		auto ipp_status=cupsFinishDestDocument(lock->http,
						       lock->dest,
						       lock->info);

		if (ipp_status != IPP_STATUS_OK)
			throw EXCEPTION(cupsLastErrorString());
	}

	job_sentry.unguard();

	auto ipp_status=cupsCloseDestJob(lock->http,
					 lock->dest,
					 lock->info,
					 job_id);

	if (ipp_status != IPP_STATUS_OK)
		throw EXCEPTION(cupsLastErrorString());
	return job_id;
}

#if 0
{
	{
#endif
	};
};
