/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "http/fdtlsserverobj.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::gnutls::http::fdtlsserverObj);

namespace LIBCXX_NAMESPACE {
	namespace gnutls {
		namespace http {
#if 0
		}
	}
}
#endif

fdtlsserverObj::fdtlsserverObj()
{
}

fdtlsserverObj::~fdtlsserverObj() noexcept
{
}

void fdtlsserverObj::run_failed(const fd &socket,
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
		{
#endif
		}
	}
}
