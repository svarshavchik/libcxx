/*
** Copyright 2013-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxx_config.h"
#include "../base/testftp.H"
#include "x/fd.H"
#include "x/gnutls/session.H"
#include "x/gnutls/credentials.H"
#include "x/property_value.H"

LIBCXX_NAMESPACE::property::value<std::string>
cert("testftpcert", "testrsa1.crt"),
	domain("domain", "example.com");

bool plain()
{
	return false;
}

LIBCXX_NAMESPACE::fdbase new_server_socket(const LIBCXX_NAMESPACE::fdbase
					   &socket)
{
	auto credentials=
		LIBCXX_NAMESPACE::gnutls::credentials::certificate::create
		("testrsa1.crt", "testrsa1.key", GNUTLS_X509_FMT_PEM);

	LIBCXX_NAMESPACE::gnutls::dhparams dh(x::gnutls::dhparams::create());
	dh->import();
	credentials->set_dh_params(dh);

	auto sess=LIBCXX_NAMESPACE::gnutls::session::create(GNUTLS_SERVER,
							    socket);

	sess->credentials_set(credentials);
	sess->set_default_priority();

	int direction;

	if (!sess->handshake(direction))
		throw EXCEPTION("Handshake did not complete");

	return sess;
}

void shutdown_server_socket(const LIBCXX_NAMESPACE::fdbase &socket)
{
	int ignore;

	if (!LIBCXX_NAMESPACE::gnutls::session(socket)->bye(ignore))
	{
		throw EXCEPTION("bye() failed");
	}
}

LIBCXX_NAMESPACE::ftp::client new_client(const LIBCXX_NAMESPACE::fdbase
					 &connArg, bool passive)
{
	auto credentials=
		LIBCXX_NAMESPACE::gnutls::credentials::certificate::create();

	credentials->set_x509_trust_file(cert.get(), GNUTLS_X509_FMT_PEM);

	return LIBCXX_NAMESPACE::ftp::client::create(connArg,
						     credentials,
						     domain.get(),
						     passive);
}

LIBCXX_NAMESPACE::ftp::client new_client(const LIBCXX_NAMESPACE::fdbase
					 &connArg,
					 const LIBCXX_NAMESPACE::fdtimeout
					 &config,
					 bool passive)
{
	auto credentials=
		LIBCXX_NAMESPACE::gnutls::credentials::certificate::create();

	credentials->set_x509_trust_file(cert.get(), GNUTLS_X509_FMT_PEM);

	return LIBCXX_NAMESPACE::ftp::client::create(connArg,
						     credentials,
						     config,
						     domain.get(),
						     passive);
}
