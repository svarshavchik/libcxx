/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/pwd.H"
#include "x/sysexception.H"
#include <unistd.h>
#include <cstring>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

passwd::passwd(const std::string &username)
{
	init();

	struct ::passwd *res;

	if (getpwnam_r(username.c_str(), &pwstruct, &buffer[0], buffer.size(),
		       &res) < 0)
		throw SYSEXCEPTION("getpwnam_r");

	if (!res)
		memset(&pwstruct, 0, sizeof(pwstruct));
}

passwd::passwd(uid_t userid)
{
	init();

	struct ::passwd *res;

	if (getpwuid_r(userid, &pwstruct, &buffer[0], buffer.size(), &res) < 0)
		throw SYSEXCEPTION("getpwuid_r");

	if (!res)
		memset(&pwstruct, 0, sizeof(pwstruct));
}


void passwd::init()
{
	long n=sysconf(_SC_GETPW_R_SIZE_MAX);

	if (n < 0)
		n=8192;

	buffer.resize(n);
}

passwd::~passwd()
{
}

#if 0
{
#endif
}
