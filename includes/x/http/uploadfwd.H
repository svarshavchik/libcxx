/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_uploadfwd_H
#define x_http_uploadfwd_H

#include <x/ptrfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

class uploadObj;

//! A collection of files to upload with a form

//! \code
//! auto files=INSERT_LIBX_NAMESPACE::http::upload::create();
//!
//! files->add("file", "photo.jpg");
//!
//! ua->request(INSERT_LIBX_NAMESPACE::http::POST, uri,
//!             LIBCXX_NAMESPACE::http::form::parameters::create(),
//!             "utf-8", files);
//! \endcode
//!
//! This is an alternative to manually specify files to upload via an HTTP
//! form. Instead of manually listing the files as parameters to request(),
//! instantiate this reference-counted objects, invoke the add() method one or
//! more times to add a file to the upload, then pass the upload handle
//! to request(), in lieu of an explicit file upload parameter list.
//!
typedef ref<uploadObj> upload;

//! A nullable pointer reference to an \ref upload "upload" object.

typedef ptr<uploadObj> uploadptr;

#if 0
{
#endif
}
#endif
