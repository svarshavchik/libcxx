/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_sessionfwd_H
#define x_tls_sessionfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>
#include <x/exception.H>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {

#if 0
	};
};
#endif

class sessionBase;
class sessionFactoryBase;
class sessionObj;

//! A TLS/SSL session

//! \code
//!
//! fd filedesc=netaddr::create("www.example.com", "https", SOCK_STREAM)->connect();
//!
//! gnutls::session sess(gnutls::session::create(GNUTLS_CLIENT, filedesc));
//! \endcode
//!
//! A TLS/SSL session object is instantiated by specifying either
//! \c GNUTLS_CLIENT or \c GNUTLS_SERVER, and the underlying input/output
//! sequence (a file descriptor reference is fine, but any object that
//! implements the \link fdbaseObj fdbaseObj\endlink interface).
//! The following requirements must
//! be met before reading or writing to the TLS/SSL stream:
//!
//! \code
//! sess->credentials_set(clientCred);
//! sess->set_default_priority();
//! \endcode
//!
//! The session must have a
//! \link gnutls::credentials::certificate certificate \endlink object defined,
//! and a list of acceptable encryption ciphers and algorithms.
//! \link sessionObj::set_default_priority() set_default_priority() \endlink
//! loads a default list of ciphers and algorithms.
//!
//! Use
//! \link sessionObj::set_priority() set_priority()\endlink
//! to set a list of ciphers specified as a colon-separated keyword string.
//!
//! Other optional settings:
//!
//! * \link sessionObj::set_max_size() set_max_size()\endlink sets the
//! maximum TLS/SSL record size
//!
//! * \link sessionObj::server_set_certificate_request() server_set_certificate_request()\endlink
//! requests a client certificate (server only). The subsequent handshake
//! fails if the client does not provide a certificate.
//!
//! * \link sessionObj::set_private_extensions() set_private_extensions()\endlink enables private GnuTLS extensions
//!
//! * On the client side,
//! \link sessionObj::set_server_name() set_server_name() \endlink
//! sets the hostname the client is connecting to.
//!
//! After completing the initialization, use
//! \link sessionObj::handshake() handshake() \endlink
//! to establish the encrypted connection.
//! An explicit call to handshake() is not required, as it will be done
//! automatically, the first time send() or recv() as call. An explicit call
//! to handshake() enables clients and servers to verify each other's
//! credentials, since credentials are only available after a handshake
//! completes.
//!
//! \code
//! gnutls::session sess(gnutls::session::base::client("www.example.com", "https"));
//! \endcode
//!
//! The
//! \ref gnutls::sessionBase::client() "session::base::client()" is a
//! convenience method that creates a connection to the server using the
//! default list of certificate authorities, and verifies the server's
//! certificate.
//!
//! When verifying the server's certificate manually, clients should use
//! \link sessionObj::verify_peer(const std::string &) verify_peer(hostname) \endlink
//! to validate that the server supplied the correct certificate.
//! Servers should invoke
//! \link sessionObj::verify_peer() verify_peer() \endlink
//! to validate that the client supplied an acceptable certificate.
//! Both clients and servers may use
//! \link sessionObj::get_peer_certificates() get_peer_certificates() \endlink
//! to retrieve the peer's actual certificate, for closer examination, and
//! \link sessionObj::get_max_size() get_max_size() \endlink to obtain the
//! largest packet size that can be sent or received.
//!
//! After a succesful handshake, use
//! \link sessionObj::send() send() \endlink and
//! \link sessionObj::recv() recv() \endlink to write or read to the session
//! object.
//! \link sessionObj::read_pending() read_pending() \endlink checks if
//! unread data is available.
//! \link sessionObj::alert_send() alert_send() \endlink sends an alert packet
//! to the peer,
//! \link sessionObj::rehandshake() rehandshake() \endlink sends a rehandshake
//! request. Finally,
//! \link sessionObj::bye() bye() \endlink shuts down the TLS/SSL session.
//!
//! For convenience, the session object implements the
//! \ref fdbaseObj "file descriptor interface" methods pubread() and pubwrite().
//! An iostream may be obtained via getiostream().
//! The iostream object may be used
//! effectively only when the underlying SSL/TLS transport uses blocking
//! sockets. Applications that use non-blocking sockets must manage
//! reading and writing themselves, and cannot use iostream.
//!
//! When using non-blocking sockets, these functions may fail due to
//! required I/O.
//! See their documentation for more information, and check the value of the
//! \c direction parameter to determine
//! whether the application should wait for the underlying sequence to be
//! readable or writable.
//!
//! \par Thread safety
//!
//! A session object is not thread safe, with one exception:
//!
//! After a handshake completes,
//! \link sessionObj::send() send() \endlink and
//! \link sessionObj::recv() recv() \endlink have limited thread safety:
//! different threads may invoke send() and recv(), however one thread
//! can use send() at a time, and one thread can use recv() at a time.
//! \link sessionObj::bye() bye() \endlink is not thread safe, so TLS
//! connection shut down is not thread safe.

typedef ref<sessionObj, sessionBase> session;

//! A nullable pointer reference to a session object

//! \see session
typedef ptr<sessionObj, sessionBase> sessionptr;

#if 0
{
	{
#endif
	}
}
#endif
