/*
** Copyright 2013-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "../base/ftp_client.H"
#include "x/fdtimeoutconfig.H"
#include "x/gnutls/session.H"
#include "x/gnutls/credentials.H"

namespace LIBCXX_NAMESPACE {
	namespace ftp {
#if 0
	}
}
#endif

clientObj::impl_tls_extraObj
::impl_tls_extraObj(const fdbase &socket,
		    const std::string &server_nameArg,
		    const gnutls::credentials::certificate &certificateArg)
	: certificate(certificateArg),
	  session(handshake(socket, certificateArg, server_nameArg, nullptr)),
	  session_data(session->get_session_data()),
	  server_name(server_nameArg)
{
}

clientObj::impl_tls_extraObj::~impl_tls_extraObj() noexcept
{
}

fdbase clientObj::impl_tls_extraObj::handshake(const fdbase &socket)
{
	return handshake(socket, certificate, server_name, &session_data);
}

void clientObj::impl_tls_extraObj::shutdown()
{
	shutdown(session);
}

void clientObj::impl_tls_extraObj::shutdown(const fdbase &socket)
{
	int ignore;
	bool flag;

	// If the transport is broken, ignore any further exceptions in bye().

	try {
		flag=gnutls::session(socket)->bye(ignore);
	} catch (...) {
		flag=true;
	}

	if (!flag)
		throw EXCEPTION("TLS shutdown did not complete");
}

fdbase clientObj::impl_tls_extraObj::setTransport(const fdbase &socket)
{
	session->setTransport(socket);
	return session;
}

gnutls::session clientObj::impl_tls_extraObj
::handshake(const fdbase &socket,
	    const gnutls::credentials::certificate &credentials,
	    const std::string &server_name,
	    const gnutls::datum_t *session_data)
{
	auto session=gnutls::session::create(GNUTLS_CLIENT, socket);

	session->credentials_set(credentials);
	session->set_default_priority();
	session->set_private_extensions();

	if (!server_name.empty())
		session->set_server_name(server_name);

	if (session_data)
		session->set_session_data(*session_data);

	int ignore;

	if (!session->handshake(ignore))
		throw EXCEPTION("TLS shutdown did not complete");

	if (!server_name.empty())
		session->verify_peer(server_name);
	else
		session->verify_peer();
	return session;
}


clientObj::clientObj(const fdbase &connArg,
		     const gnutls::credentials::certificate &credentialsArg,
		     const std::string &server_nameArg,
		     bool passiveArg)
	: clientObj(connArg, passiveArg)
{
	auth_tls(connArg, credentialsArg, server_nameArg);
}

clientObj::clientObj(const fdbase &connArg,
		     const gnutls::credentials::certificate &credentialsArg,
		     const fdtimeout &timeoutArg,
		     const std::string &server_nameArg,
		     bool passiveArg)
	: clientObj(connArg, timeoutArg, passiveArg)
{
	auth_tls(timeoutArg, credentialsArg, server_nameArg);
}

void clientObj::auth_tls(const fdbase &connArg,
			 const gnutls::credentials::certificate &credentialsArg,
			 const std::string &server_nameArg)
{
	lock l(conn);

	auth_tls(l);

	l->set_default_timeouts();
	l->tls=impl_tls::create(l->current_timeout, server_nameArg,
				credentialsArg);
	l->stream=l->tls->session->getiostream();
	l->stream->exceptions(std::ios::badbit);
	l->cancel_default_timeouts();
}

#if 0
{
	{
#endif
	}
}
