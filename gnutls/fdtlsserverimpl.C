/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/http/fdtlsserverimpl.H"
#include "x/timerfd.H"
#include "x/hms.H"

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

property::value<hms>
gnutls::http::fdtlsserverimpl::handshake_timeout(LIBCXX_NAMESPACE_STR
						 "::https::server::handshake_timeout",
						 hms(0,5,0));

property::value<hms>
gnutls::http::fdtlsserverimpl::bye_timeout(LIBCXX_NAMESPACE_STR
					   "::https::server::bye_timeout",
					   30);

gnutls::http::fdtlsserverimpl::fdtlsserverimpl()
{
}

gnutls::http::fdtlsserverimpl::~fdtlsserverimpl()
{
}

fdbase gnutls::http::fdtlsserverimpl::filedesc()
{
	return sess;
}

class gnutls::http::fdtlsserverimpl::cleanup_helper {

public:

	fdtlsserverimpl *p;

	cleanup_helper(fdtlsserverimpl *pArg) : p(pArg)
	{
	}

	~cleanup_helper()
	{
		p->clear();
	}
};

void gnutls::http::fdtlsserverimpl::run(const fd &connection,
					const fd &terminator,
					const session &sessionArg)
{
	cleanup_helper helper(this);

	sess=sessionArg;

	LIBCXX_NAMESPACE::http::fdserverimpl::run(connection, terminator);
}

void gnutls::http::fdtlsserverimpl::clear()
{
	sess=sessionptr();
	LIBCXX_NAMESPACE::http::fdserverimpl::clear();
}

void gnutls::http::fdtlsserverimpl::filedesc_installed(const fdbase &transport)
{
	sess->setTransport(transport);

	set_readwrite_timeout(handshake_timeout.get().seconds());

	int dummy;

	if (!sess->handshake(dummy))
		throw EXCEPTION("TLS negotiation failed");

	cancel_readwrite_timeout();
}

void gnutls::http::fdtlsserverimpl::ran()
{
	set_readwrite_timeout(bye_timeout.get().seconds());
	try {
		int dummy;

		sess->bye(dummy);
	} catch (...) {
	}
	cancel_readwrite_timeout();
}

#if 0
{
#endif
};
