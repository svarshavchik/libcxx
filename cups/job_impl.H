/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef job_impl_h
#define job_impl_h

#include "libcxx_config.h"
#include "x/ref.H"
#include "x/obj.H"
#include "x/cups/job.H"
#include "x/mpobj.H"
#include "destination_impl.H"
#include <cups/cups.h>

namespace LIBCXX_NAMESPACE {
	namespace cups {
#if 0
	};
};
#endif

class LIBCXX_HIDDEN job_implObj : public jobObj {

 public:

	const ref<destination_implObj> destination;

	struct document_info {
		std::string name;
		document_t document;
	};

	struct job_info_s {
		int num_options=0;
		cups_option_t *options=nullptr;

		std::vector<document_info> documents;
	};

	typedef mpobj<job_info_s> job_info_t;

	mutable job_info_t job_info;

	job_implObj(const ref<destination_implObj> &destination);

	~job_implObj();


	void set_option(const std::string_view &name,
			const std::string_view &value) override;

	void set_option(const std::string_view &name, int value) override;

	void add_document(const std::string &name,
			  const document_t &document) override;

	int submit(const std::string_view &title) override;
};

#if 0
{
	{
#endif
	};
};

#endif
