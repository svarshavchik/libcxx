/*
** Copyright 2013-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ftp_exception_H
#define x_ftp_exception_H

#include <x/namespace.h>
#include <x/ftp/exceptionimpl.H>

namespace LIBCXX_NAMESPACE {

	namespace ftp {
#if 0
	}
}
#endif

//! What gets thrown in response to errors from the FTP server

//! \code
//! ...
//! catch (const INSERT_LIBX_NAMESPACE::ftp::exception &e)
//! {
//!      int n=e->status_code;
//!
//!      std::cerr << e << std::endl;
//! }
//! \endcode
//!
//! \c status_code is the three-digit error code returned from the FTP server.
typedef custom_exception<ftp_custom_exception> exception;

#if 0
{
	{
#endif
	}
}
#endif
