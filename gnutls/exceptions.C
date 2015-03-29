/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/gnutls/exceptions.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

gnutls::errexception::errexception(int errcodeArg,
				   const char *funcnameArg)
	: errcode(errcodeArg)
{
	(*this) << std::string(funcnameArg) << ": "
		<< gnutls_strerror(errcode);
	done();
}

gnutls::errexception::~errexception() noexcept
{
}

gnutls::alertexception::alertexception(bool fatalArg,
				       gnutls_alert_description_t alertcodeArg)
	: fatal(fatalArg), alertcode(alertcodeArg)
{
	(*this) << gnutls_alert_get_name(alertcode);
	done();
}

gnutls::alertexception::~alertexception() noexcept
{
}

#if 0
{
#endif
}
