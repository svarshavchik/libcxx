/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/grp.H"
#include "x/sysexception.H"
#include <unistd.h>
#include <cstring>
#include <algorithm>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

group::group(const std::string &groupname)
{
	init();

	struct ::group *res;

	if (getgrnam_r(groupname.c_str(), &grstruct, &buffer[0], buffer.size(),
		       &res) < 0)
		throw SYSEXCEPTION("getpwnam_r");

	if (!res)
		memset(&grstruct, 0, sizeof(grstruct));
}

group::group(gid_t groupid)
{
	init();

	struct ::group *res;

	if (getgrgid_r(groupid, &grstruct, &buffer[0], buffer.size(), &res) < 0)
		throw SYSEXCEPTION("getpwuid_r");

	if (!res)
		memset(&grstruct, 0, sizeof(grstruct));
}


void group::init()
{
	long n=sysconf(_SC_GETGR_R_SIZE_MAX);

	if (n < 0)
		n=8192;
	buffer.resize(n);
}

group::~group()
{
}

void group::getgroups(std::vector<gid_t> &groups)
{
	int n= ::getgroups(0, NULL);

	if (n < 0)
		throw SYSEXCEPTION("getgroups");

	if (n == 0)
	{
		groups.resize(1);
		groups[0]=getegid();
		return;
	}

	gid_t buf[n+1];

	n= ::getgroups(n, buf+1);

	if (n < 0)
		throw SYSEXCEPTION("getgroups");

	++n;

	std::sort(buf+1, buf+n);

	buf[0]=getegid();
	gid_t *last=std::unique(buf, buf+n);

	groups.resize(last-buf+1);
	std::copy(buf, last, &groups[1]);
}

#if 0
{
#endif
}
