/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_session_H
#define x_tls_session_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/exception.H>
#include <x/fd.H>
#include <x/netaddr.H>
#include <x/gnutls/init.H>
#include <x/gnutls/sessionobj.H>

#include <gnutls/gnutls.h>

#include <list>
#include <string>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {

#if 0
	};
};
#endif

//! Base class for a session reference

//! This is the base class for \ref session "INSERT_LIBX_NAMESPACE::gnutls::session"
//! and  \ref sessionptr "INSERT_LIBX_NAMESPACE::gnutls::sessionptr"

class sessionBase : public ptrrefBase {

public:

	//! Retrieve the list of all known session ciphers

	//! This function retrieves the list of all session ciphers implemented
	//! by GnuTLS.
	//!
	//! \code
	//! std::list<gnutls_cipher_algorithm_t> cipher_list;
	//!
	//! gnutls::session::get_cipher_list(cipher_list);
	//! \endcode
	//!
	//! See also:
	//! \link get_cipher_name() gnutls::session::get_cipher_name()\endlink,
	//! \link get_cipher_id() gnutls::session::get_cipher_id()\endlink.

	static
	void get_cipher_list(//! This list gets filled with the known cipher list
				std::list<gnutls_cipher_algorithm_t> &listRet)
		noexcept;

	//! Return the name of a cipher

	//! \code
	//! std::string n=gnutls::session::get_cipher_name(GNUTLS_CIPHER_AES_256_CBC);
	//! \endcode
	//!
	//! \return the cipher's name. An %exception gets thrown if the
	//! cipher is not known.
	static std::string get_cipher_name(//! Session cipher
					   gnutls_cipher_algorithm_t a);

	//! Return the cipher id given its name

	//! \code
	//! gnutls_cipher_algorithm_t algo=
	//!     gnutls::session::get_cipher_id("AES-256-CBC");
	//! \endcode
	//! \return the cipher. An %exception gets thrown if the name is
	//! not recognized.
	static gnutls_cipher_algorithm_t get_cipher_id(//! Cipher's name
						       std::string name);

	//! Retrieve the list of all known compression methods

	//! This function retrieves the list of all compression methods
	//! implemented by GnuTLS.
	//!
	//! \code
	//! std::list<gnutls_compression_method_t> compression_list;
	//!
	//! gnutls::session::get_compression_list(compression_list);
	//! \endcode
	//!
	//! See also:
	//! \link get_compression_name() gnutls::session::get_compression_name()\endlink,
	//! \link get_compression_id() gnutls::session::get_compression_id()\endlink.

	static void get_compression_list(//! The list of known compression methods is placed here
					 std::list<gnutls_compression_method_t>
					 &listRet) noexcept;

	//! Return the name of a compression_method

	//! \code
	//! std::string n=gnutls::session_get_compression_name(GNUTLS_COMP_DEFLATE);
	//! \endcode
	//!
	//! \return the compression method's name. An %exception gets thrown if
	//! the compression method is not known.
	static std::string get_compression_name(//! Compression id
						gnutls_compression_method_t a);

	//! Return the compression method id, given its name

	//! \code
	//! gnutls_compression_method_t comp=
	//!     gnutls::session::get_compression_id("DEFLATE");
	//! \endcode
	//!
	//! \return the compression method's ID. An %exception gets thrown if
	//! the compression method is not known.

	static gnutls_compression_method_t
	get_compression_id(//! Compression name
			   std::string name);

	//! Retrieve the list of all known key exchange algorithms

	//! This function retrieves the list of all key exchange algorithms
	//! implemented by GnuTLS.
	//!
	//! \code
	//! std::list<gnutls_kx_algorithm_t> kx_list;
	//!
	//! gnutls::session::get_kx_list(kx_list);
	//! \endcode
	//!
	//! See also:
	//! \link get_kx_name() gnutls::session::get_kx_name()\endlink,
	//! \link get_kx_id() gnutls::session::get_kx_id()\endlink.

	static void get_kx_list(//! The list of known key exchange algorithms is placed here
				std::list<gnutls_kx_algorithm_t> &listRet)
		noexcept;

	//! Return the name of a key exchange algorithm

	//! \code
	//! std::string n=gnutls::session_get_kx_name(GNUTLS_KX_DHE_RSA);
	//! \endcode
	//!
	//! \return the algorithm's name. An %exception gets thrown if
	//! the key exchange algorithm is not known.
	static std::string get_kx_name(//! Algorithm ID
				       gnutls_kx_algorithm_t a);

	//! Return the key exhange algorithm id, given its name

	//! \code
	//! gnutls_kx_algorithm_t comp=
	//!     gnutls::session::get_kx_id("DHE-RSA");
	//! \endcode
	//!
	//! \return the algorithm's ID. An %exception gets thrown if
	//! the algorithm is not known.

	static gnutls_kx_algorithm_t get_kx_id(//! Algorithm's
					       std::string name);

	//! Retrieve the list of all known hash functions

	//! This function retrieves the list of all hash functions
	//! implemented by GnuTLS.
	//!
	//! \code
	//! std::list<gnutls_mac_algorithm_t> mac_list;
	//!
	//! gnutls::session::get_mac_list(mac_list);
	//! \endcode
	//!
	//! See also:
	//! \link get_mac_name() gnutls::session::get_mac_name()\endlink,
	//! \link get_mac_id() gnutls::session::get_mac_id()\endlink.

	static void get_mac_list(//! The list of known hash functions is placed here
				 std::list<gnutls_mac_algorithm_t> &listRet)
		noexcept;

	//! Return the name of a hash function

	//! \code
	//! std::string n=gnutls::session_get_mac_name(GNUTLS_MAC_MD5);
	//! \endcode
	//!
	//! \return the hash's name. An %exception gets thrown if
	//! the hash function is not known.
	static std::string get_mac_name(//! Hash function ID
					gnutls_mac_algorithm_t a);

	//! Return the hash function ID, given its name

	//! \code
	//! gnutls_mac_algorithm_t comp=
	//!     gnutls::session::get_mac_id("SHA512");
	//! \endcode
	//!
	//! \return the hash function's id. An %exception gets thrown if
	//! the algorithm is not known.

	static gnutls_mac_algorithm_t get_mac_id(//! Algorithm's
						 std::string name);

	//! This function retrieves the list of all SSL/TLS protocols
	//! implemented by GnuTLS.
	//!
	//! \code
	//! std::list<gnutls_protocol_t> protocol_list;
	//!
	//! gnutls::session::get_protocol_list(protocol_list);
	//! \endcode
	//!
	//! See also:
	//! \link get_protocol_name() gnutls::session::get_protocol_name()\endlink,
	//! \link get_protocol_id() gnutls::session::get_protocol_id()\endlink.

	static void get_protocol_list(//! The list of known protocols is placed here
				      std::list<gnutls_protocol_t> &listRet)
		noexcept;

	//! Return the name of a protocol

	//! \code
	//! std::string n=gnutls::session_get_protocol_name(GNUTLS_PROTOCOL_TLS1_0);
	//! \endcode
	//!
	//! \return the protocol's name. An %exception gets thrown if
	//! the protocol is not known.
	static std::string get_protocol_name(//! Protocol ID
					     gnutls_protocol_t a);

	//! Return the protocol ID, given its name

	//! \code
	//! gnutls_protocol_t comp=
	//!     gnutls::session::get_protocol_id("TLS1.0");
	//! \endcode
	//!
	//! \return the protocol ID. An %exception gets thrown if
	//! the protocol is not known.

	static gnutls_protocol_t get_protocol_id(//! Algorithm's
						 std::string name);

	//! This function retrieves the list of all certificate types
	//! implemented by GnuTLS.
	//!
	//! \code
	//! std::list<gnutls_certificate_type_t> certificate_list;
	//!
	//! gnutls::session::get_certificate_list(certificate_list);
	//! \endcode
	//!
	//! See also:
	//! \link get_certificate_name() gnutls::session::get_certificate_name()\endlink,
	//! \link get_certificate_id() gnutls::session::get_certificate_id()\endlink.

	static void get_certificate_list(//! The list of known certificate types is placed here
					 std::list<gnutls_certificate_type_t>
					 &listRet) noexcept;

	//! Return the name of a certificate type

	//! \code
	//! std::string n=gnutls::session_get_certificate_name(GNUTLS_CRT_X509);
	//! \endcode
	//!
	//! \return the certificate type's name. An %exception gets thrown if
	//! the certificate type is not known.
	static std::string get_certificate_name(//! Certificate ID
						gnutls_certificate_type_t a);

	//! Return the certificate type ID, given its name

	//! \code
	//! gnutls_certificate_type_t comp=
	//!     gnutls::session::get_certificate_id("X.509");
	//! \endcode
	//!
	//! \return the certificate type ID. An %exception gets thrown if
	//! the certificate is not known.

	static gnutls_certificate_type_t get_certificate_id(//! Algorithm's
							    std::string name);

	//! Create a new client session and connect to the server

	//! This convenience method creates a new session with all the
	//! default settings, and connects to a server.
	//!
	//! The session uses the default list of certificate authorities,
	//! and the server's certificate gets verified.
	static session client(//! Server's hostname
			      const std::string &server,

			      //! Server's port
			      const std::string &port)
	{
		return client(netaddr::create(server, port)->connect(),
			      server);
	}

	//! Create a new client session and connect to the server

	//! \overload
	//!
	static session client(//! Server's hostname
			      const std::string &server,

			      //! Server's port
			      int port)
	{
		return client(netaddr::create(server, port)->connect(),
			      server);
	}

	//! Create a new client session and connect to the server

	//! \overload
	//!

	static session client(//! Existing socket connection
			      const fd &conn,

			      //! Server's name. The server's certificate name must match it.
			      const std::string &server);

	//! A reference to a session factory object

	//! Refer to this type as \c gnutls::session::base::factory.
	//! \see gnutls::sessionObj::factoryObj

	typedef ref<sessionObj::factoryObj> factory;

	//! A nullable pointer reference to a session factory object

	//! Refer to this type as \c gnutls::session::base::factoryptr.
	//! \see gnutls::sessionObj::factoryObj

	typedef ptr<sessionObj::factoryObj> factoryptr;
};

#if 0
{
	{
#endif
	}
}
#endif
