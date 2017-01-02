/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/servent.H"
#include "x/sysexception.H"
#include <cstring>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

servent::servent(const std::string &servname,
		 const std::string &proto)
{
	init();

	struct ::servent *res;

	while (getservbyname_r(servname.c_str(), proto.c_str(),
			       &servstruct, &buffer[0], buffer.size(), &res)
	       < 0)
	{
		if (errno != ERANGE)
			throw SYSEXCEPTION("getservbyname_r");

		buffer.resize(buffer.size()+1024);
	}

	if (!res)
		memset(&servstruct, 0, sizeof(servstruct));
}

servent::servent(int port,
		 const std::string &proto)
{
	init();

	struct ::servent *res;

	while (getservbyport_r(port, proto.c_str(),
			       &servstruct, &buffer[0], buffer.size(), &res)
	       < 0)
	{
		if (errno != ERANGE)
			throw SYSEXCEPTION("getservbyport_r");

		buffer.resize(buffer.size()+1024);
	}

	if (!res)
		memset(&servstruct, 0, sizeof(servstruct));
}


void servent::init()
{
	buffer.resize(1024);
}

servent::~servent()
{
}

#if 0
{
#endif
}
