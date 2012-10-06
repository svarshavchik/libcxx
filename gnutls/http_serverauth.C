/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/serverauth.H"

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif

std::string serverauthBase::compute_a1(gcry_md_algos algorithm,
				       const std::string &userid,
				       const std::string &password,
				       const std::string &realm)
{
	std::string a1_str = userid + ":" + realm + ":" + password;

	return gcrypt::md::base::hexdigest(a1_str.begin(),
					   a1_str.end(),
					   algorithm);
}

#if 0
{
	{
#endif
	};
};

