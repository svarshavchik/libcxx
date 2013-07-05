/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_credentials_certificatefwd_H
#define x_tls_credentials_certificatefwd_H

#include <x/namespace.h>
#include <x/ptr.H>


namespace LIBCXX_NAMESPACE {

	namespace gnutls {

		//! TLS/SSL session %certificates and %credentials

		//! This namespace defines objects that store TLS/SSL
		//! session %credentials.

		namespace credentials {

#if 0
		};
	};
};
#endif

class certificateObj;
class callbackObj;

//! A reference to a %certificate %credentials object

//! This object holds %certificate and encryption information for a TLS/SSL
//! session. Each TLS/SSL \link gnutls::session session \endlink needs
//! %certificate
//! %credentials defined by this object.
//!
//! A separate %certificate object is required for each individual TLS/SSL
//! session. Although each instance gets initialized identically, after
//! the TLS/SSL handshake the %certificate object holds peer's certificate
//! data that can be examined accordingly.
//!
//! \code
//! gnutls::credentials::certificate
//!		serverCert(gnutls::credentials::certificate::create());
//!
//! std::list<gnutls::x509::crt> certlist;
//!
//! gnutls::x509::crt::import_cert_list(certlist, "server.crt",
//!					GNUTLS_X509_FMT_PEM);
//!
//! gnutls::x509::privkey certkey(gnutls::x509::privkey::create());
//!
//! certkey->import_pkcs1("server.key", GNUTLS_X509_FMT_PEM);
//!
//! serverCert->set_key(certlist, certkey);
//! \endcode
//!
//! A typical server application uses
//! \link certificateObj::set_key() set_key() \endlink
//! to load the actual %certificate and the private key into the %certificate
//! object. The
//! \link certificateObj::set_x509_keyfile() set_x509_keyfile() \endlink
//! function is an alternative short cut for performing the above steps.
//! Another alternative is
//! \link certificateObj::set_x509_keyfile() set_x509_keymem()\endlink.
//!
//! An even more open-ended alternative is to use a %certificate callback.
//! This is done by not defining the %certificate and the key, and invoking
//! the certificate credentials' object's
//! \link gnutls::credentials::certificateObj::set_callback() set_callback() \endlink
//! method to define a
//! \link gnutls::credentials::callback callback\endlink.
//!
//! \code
//! gnutls::credentials::certificate
//!		clientCert(gnutls::credentials::certificate::create());
//!
//! clientCert->set_x509_trust_default(); // Or:
//!
//! clientCert->set_x509_trust_file("cas.crt", GNUTLS_X509_FMT_PEM);
//! \endcode
//!
//! A typical client application uses
//! \link certificateObj::set_x509_trust_file() set_x509_trust_file() \endlink
//! to load trusted %certificate authorities. Alternatively,
//! \link certificateObj::set_x509_trust() set_x509_trust() \endlink
//! loads trusted %certificate authorities from a memory buffer.
//! \link certificateObj::set_x509_trust_default() set_x509_trust_default() \endlink
//! loads the trusted %certificate authorities from the system default location.
//!
//! A client that intends to supply a client certificate to the server should
//! also use the same set_key() function (or its alternatives) to load its
//! certificate into %credentials object. A server that expects to validate
//! client certificates should use the same set_x509_trust_file() to load
//! %certificate authorities.
//!
//! Both clients and servers should use
//! \link certificateObj::set_dh_params() set_dh_params() \endlink
//! to set the decryption key exchange parameters, when using DSA certificates.
//!
//! The default %certificate verification limits may be adjuted by using
//! \link certificateObj::set_verify_flags() set_verify_flags() \endlink and
//! \link certificateObj::set_verify_limits() set_verify_limits()\endlink.

typedef ref<certificateObj> certificate;

//! A nullable pointer reference to a credentials object.

//! \see certificate

typedef ptr<certificateObj> certificateptr;

//! Credentials callback object

//! Advanced TLS/SSL applications may not use a predetermined %certificate,
//! but rather select one based on the peer's requested %credentials.
//! This is done via a callback mechanism.
//!
//! The application must subclass
//! \link callbackObj gnutls::certificate::callbackObj \endlink and
//! define the \link callbackObj::get() get() \endlink
//! method:
//!
//! \code
//! class credCallback : public gnutls::credentials::callbackObj {
//!
//! public:
//!	credCallback() noexcept;
//!	~credCallback() noexcept;
//!
//!	keycertptr get(const gnutls::sessionObj *session,
//!                    const std::list<std::string> &hostname_list,
//!		       const std::vector<gnutls_pk_algorithm_t> &algos,
//!		       const gnutls_datum_t *req_ca_dn,
//!		       size_t n_req_ca_dn);
//! };
//!
//! credCallback::credCallback() noexcept
//! {
//! }
//!
//! credCallback::keycertptr credCallback::get(const gnutls::sessionObj *session,
//!                                            const std::list<std::string> &hostname_list,
//!                                            const std::vector<gnutls_pk_algorithm_t> &algos,
//!                                            const gnutls_datum_t *req_ca_dn)
//! {
//!     keycert kc=keycert::create();
//!
//!	gnutls::x509::crt::import_cert_list(kc->cert, "server.crt",
//!					    GNUTLS_X509_FMT_PEM);
//!
//!     kc->key->import_pkcs1("server.key", GNUTLS_X509_FMT_PEM);
//! }
//! \endcode
//!
//! The above example does not do anything more than loading a single
//! %certificate and key. Advanced TLS/SSL application will examine the contents
//! of the other parameters before selecting one of several available
//! certificates.
//!
//! This callback mechanism is for both clients and servers.
//! The first parameter is the native pointer to the session object of the
//! \c TLS session that is being authenticated. A callback method can only
//! be invoked if the session object is already protected by a reference
//! held elsewhere.
//!
//! \c hostname_list is always an empty list on the client side. On the
//! server side \c hostname_list may be a non-empty list if the client
//! uses a TLS extension that indicates which %certificate it expects. The
//! server then proceeds to select the right %certificate.
//!
//! Client callbacks may look at \c algos, the list of signature algorithms
//! the server supports, and select the appropriate certificate. This parameter
//! is always an empty list, for server callbacks. \c req_ca_dn is always
//! NULL for server callbacks, for client callbacks it may or may not be NULL.
//! If the server requested
//! a client certificate, the client callback receives a non-NULL req_ca_dn
//! pointing to an array of raw DER-encoded RDNs. This is a list of
//! acceptable signers for the client's certificate. The number of signers
//! is given in n_req_ca_dn. See gnutls::rdn for more information.
//!
//! The callback function must return a \c keycertptr, which is a reference
//! to a reference-counted object with two fields:
//!
//! - key: a private key. The constructor creates a new \c key object, by
//! default, or a prepackaged one can be specified as the optional parameter
//! to create().
//!
//! - cert: a certificate chain that goes with the key.
//!
//! Server callbacks must always return a \c keycert, but client callback can
//! return a null \c keycertptr, if the client does not wish to provide
//! a certificate.
//!
//! \par Instantiating the callback object
//!
//! \code
//! ptr<credCallback> cred(ptr<credCallback>::create());
//!
//! sess->credentials_set(serverCert);
//! cred->set_callback(cred);
//! \endcode
//!
//! A reference to the subclass must be instantiated (because the base
//! class is a reference-counted object, it should be instantiated accordingly).
//! Afterwards, use
//! \link gnutls::credentials::certificateObj::set_callback() set_callback() \endlink
//! on the credentials object to define the callback. The callback function
//! then gets invoked during
//! \link gnutls::sessionObj::handshake() handshake() \endlink.

typedef ref<callbackObj> callback;

//! A nullable pointer reference to a credentials callback object.

typedef ptr<callbackObj> callbackptr;

#if 0
{
	{
		{
#endif
		}
	}
}
#endif
