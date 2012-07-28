/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "http/useragent.H"
#include "http/fdtlsclientobj.H"
#include "gnutls/credentials.H"

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

http::useragentObj::idleconn
http::useragentObj::init_https_socket(clientopts_t opts,
				      const std::string &host,
				      const fd &socket,
				      const fdptr &terminator)
{
	ref<connObj<gnutls::http::fdtlsclientObj> >
		newclient(ref<connObj<gnutls::http::fdtlsclientObj> >
			  ::create());

	gnutls::credentials::certificate
		cert(gnutls::credentials::certificate::create());

	cert->set_x509_trust_default();

	gnutls::session sess(gnutls::session::create(GNUTLS_CLIENT,
						     socket));
	sess->credentials_set(cert);
	sess->set_default_priority();
	sess->set_private_extensions();

	newclient->install(sess, socket, terminator, opts);
	return newclient;
}

void http::useragentBase::https_enable() noexcept
{
	(void)&http::useragentObj::init_https_socket;
}

#if 0
{
#endif
};
