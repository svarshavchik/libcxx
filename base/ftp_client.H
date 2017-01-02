/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include "libcxx_config.h"
#include "x/ftp/client.H"
#include "x/gnutls/credentials.H"
#include "x/gnutls/datumwrapper.H"

namespace LIBCXX_NAMESPACE {
	namespace ftp {
#if 0
	};
};
#endif

// Conveniently construct a lock on the implementation object

class clientObj::lock : public mpobj<impl>::lock {

public:
	lock(const conn_info &c) : mpobj<impl>::lock(c->instance)
	{
	}

	~lock() {}
};

struct clientObj::impl_tls_extraObj : virtual public obj {

public:

	// Certificate information
	gnutls::credentials::certificate certificate;

	// Control channel TLS session, when available.
	gnutls::session session;

	// Session data
	gnutls::datum_t session_data;

	// Verify server name, if not empty
	std::string server_name;

	impl_tls_extraObj(const fdbase &socket,
			  const std::string &server_nameArg,
			  const gnutls::credentials::certificate
			  &certificateArg);
	virtual ~impl_tls_extraObj();

	// Virtual linkage for everything, so that libcxx does not need
	// to link to libcxxtls.
	// handshake() really returns a gnutls::session. shutdown() really
	// takes a gnutls:session for a parameter.

	virtual fdbase handshake(const fdbase &socket);

	virtual void shutdown();

	virtual void shutdown(const fdbase &socket);

	virtual fdbase setTransport(const fdbase &socket);

	static gnutls::session handshake(const fdbase &socket,
					 const gnutls::credentials::certificate
					 &credentials,
					 const std::string &server_name,
					 const gnutls::datum_t *session_data);
};

class LIBCXX_HIDDEN clientObj::tls_socket : public fdbase {

 public:

	impl_tlsptr tlsptr;

	fdbase origsocket;

	tls_socket(fdbase &&socketArg, fdbase &&origsocketArg,
		   const impl_tlsptr &tlsptrArg);
	~tls_socket();

	void shutdown();

	using fdbase::operator->;
	using fdbase::operator *;
};

#if 0
{
	{
#endif
	}
}

#endif
