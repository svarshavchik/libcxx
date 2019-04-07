/*
** Copyright 2013-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/ftp/exception.H"

namespace LIBCXX_NAMESPACE {

	template class custom_exception<ftp::ftp_custom_exception>;

	namespace ftp {
#if 0
	};
};
#endif

ftp_custom_exception::ftp_custom_exception(int status_codeArg)
	: status_code(status_codeArg)
{
}

ftp_custom_exception::~ftp_custom_exception()
{
}

#if 0
{
	{
#endif
	}
}
