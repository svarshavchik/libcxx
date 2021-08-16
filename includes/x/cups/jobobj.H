/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_cups_jobobj_H
#define x_cups_jobobj_H

#include <x/cups/jobfwd.H>
#include <x/cups/destinationfwd.H>
#include <x/obj.H>
#include <string_view>
#include <functional>
#include <tuple>
#include <optional>

namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

//! \ref job "Job" document data from an arbitrary source.

typedef std::function<std::tuple<std::string,
				 std::function<
					 std::optional<std::string> ()>>
		      ()> document_t;

//! Implementation object.

//! \see job

class jobObj : virtual public obj {

public:

	//! Constructor
	jobObj() LIBCXX_HIDDEN;

	//! Destructor
	~jobObj() LIBCXX_HIDDEN;

	//! Set a job option.

	virtual void set_option(const std::string_view &name,
				const std::string_view &value)=0;

	//! Set a job option.
	virtual void set_option(const std::string_view &name,
				int value)=0;

	//! Set a job option.

	virtual void set_option(const std::string_view &name,
				const option_values_t &values)=0;

	//! Add a document to the job.

	virtual void add_document(const std::string &name,
				  const document_t &document)=0;

	//! Add a file to the job.
	void add_document_file(const std::string &name,
			       const std::string &filename,
			       const std::string &mime_type);

	//! Add a file to the job.

	//! The MIME type is inferred from the file.

	void add_document_file(const std::string &name,
			       const std::string &filename);

	//! Submit the print job.

	virtual int submit(const std::string_view &title)=0;
};

#if 0
{
#endif
}
#endif
