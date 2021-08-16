/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_cups_jobfwd_H
#define x_cups_jobfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE::cups {
#if 0
}
#endif

class jobObj;
class jobBase;

//! A print job.

//! A \ref destination "print destination"'s create_job() method
//! creates this job object. Various methods set the job properties,
//! then the print job gets submit()ed.
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
//!                                 // ...
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
//!
//! \par Using add_document()
//!
//! add_document() is an interface for specifying the contents of a print
//! document from some arbitrary source, instead of a discrete file.
//!
//! add_document() receives a closure, or a callable object, as a parameter.
//! The closure gets invoked by submit(), before sending the print
//! The closure returns a tuple with two values: a MIME type, and
//! a second closure. The second closure takes no parameters and returns a
//! \c std::optional<string>.
//!
//! The MIME type specifies the document's type, and the second closure gets
//! repeatedly invoked in order to obtain the document's contents. Each call
//! to the second closure returns the next chunk of the document's data.
//! The second closure returns a \c std::nullopt to indicate no more data
//! to print.
//!
//! Invoking submit() again invokes the first closure again.


typedef ref<jobObj> job;

//! A nullable pointer reference to a job printer.

typedef ptr<jobObj> jobptr;

//! A constant job printer object.

typedef const_ref<jobObj> const_job;

//! A nullable pointer reference to a const job printer object.

typedef const_ptr<jobObj> const_jobptr;

#if 0
{
#endif
}
#endif
