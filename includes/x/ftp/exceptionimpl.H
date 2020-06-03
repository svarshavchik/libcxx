/*
** Copyright 2013-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ftp_exceptionimpl_H
#define x_ftp_exceptionimpl_H

#include <x/namespace.h>
#include <x/exception.H>

namespace LIBCXX_NAMESPACE::ftp {
#if 0
}
#endif

//! Custom exception class, implements status_code

class ftp_custom_exception {

public:
	//! Conveniently parsed status code
	int status_code;

	//! Constructor
	ftp_custom_exception(int);

	//! Destructor
	~ftp_custom_exception();
};

#if 0
{
#endif
}

namespace LIBCXX_NAMESPACE {
	extern template class custom_exception<ftp::ftp_custom_exception>;
};

#endif
