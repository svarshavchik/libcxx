/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/fdserverobj.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::http::fdserverObj);

namespace LIBCXX_NAMESPACE {
	namespace http {
#if 0
	};
};
#endif


fdserverObj::fdserverObj()
{
}

fdserverObj::~fdserverObj()
{
}

void fdserverObj::run_failed(const fd &socket,
			     const exception &e)
{
	std::string sockname="socket";

	try {
		sockname= *socket->getpeername();
	} catch (...)
	{
	}

	LOG_ERROR(sockname << ": " << e);
}

#if 0
{
	{
#endif
	}
}
