/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "fdreadlimit.H"

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Constructor

fdreadlimitObj::fdreadlimitObj(const fdbase &fdptr)
	: fdbaseObj::adapterObj(fdptr), read_limit(0), read_limit_reached(false)
{
}

fdreadlimitObj::~fdreadlimitObj() noexcept
{
}

void fdreadlimitObj::set_read_limit(size_t read_limitArg,
				    const std::string &exceptionmsgArg)

{
	read_limit=read_limitArg;
	read_limit_reached=false;
	exceptionmsg=exceptionmsgArg;
}


size_t fdreadlimitObj::pubread(char *buffer, size_t cnt)

{
	if (read_limit_reached)
	{
	err:
		throw EXCEPTION(exceptionmsg);
	}

	size_t n=fdptr->pubread(buffer, cnt);

	if (read_limit && n >= read_limit)
	{
		read_limit_reached=true;
		read_limit=0;
		goto err;
	}
	read_limit -= n;
	return n;
}

#if 0
{
#endif
}
