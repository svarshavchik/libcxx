/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_cups_jobfwd_H
#define x_cups_jobfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {
	namespace cups {
#if 0
	}
}
#endif

class jobObj;
class jobBase;

//! A print job.

//! A \ref destination "print destination"'s create_job() method
//! creates this job object. Various method set the job properties
//! and the print job gets submit()ed.
//!
//! \code
//! job->set_option("copies", 2);
//!
//! job->set_option("media", "na_letter_8.5x11in");
//! \endcode
//!
//! set_option() sets the various options for this print job. The various
//! option names are defined in the \c <cups.h> header file.
//!
//! \code
//! job->add_document_file("letter", "letter.pdf");
//!
//! job->add_document
//!     ("letter",
//!      []
//!      {
//!          return std::tuple{"application/octet-stream",
//!                            []
//!                            {
//!                                 std::option<std::string> ret;
//!
//!                                 return ret;
//!                            }};
//!      });
//! \endcode
//!
//! add_document_file() adds a file to the print job.
//! add_document() adds a new \ref document_t "document" from some arbitrary
//! data source.
//!
//! \code
//! int job_id=job->submit("title");
//! \endcode
//!
//! submit() schedules the print job using the specified options and
//! documents. An exception gets thrown if the print job could not be
//! submitted.
//!
//! Each call to submit() schedules a print job. Invoking submit() a second
//! time creates a duplicate print job; but note that any manually-specified
//! documents (using add_document() to define an artbirary document data
//! source) results in the document's closure getting invoked again, which
//! may return different data.

typedef ref<jobObj> job;

//! A nullable pointer reference to a job printer.

typedef ptr<jobObj> jobptr;

//! A constant job printer object.

typedef const_ref<jobObj> const_job;

//! A nullable pointer reference to a const job printer object.

typedef const_ptr<jobObj> const_jobptr;

#if 0
{
	{
#endif
	}
}
#endif
