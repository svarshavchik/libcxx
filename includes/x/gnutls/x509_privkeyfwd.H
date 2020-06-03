/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_x509_privkeyfwd_H
#define x_tls_x509_privkeyfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

//! X.509 certificates

namespace LIBCXX_NAMESPACE::gnutls::x509 {
#if 0
}
#endif

class privkeyObj;
class privkeyBase;

//! A private key for X509 certificates

//! This is a reference to an object that holds an X509 private key.
//! \code
//! INSERT_LIBX_NAMESPACE::gnutls::x509::privkey pk(INSERT_LIBX_NAMESPACE::gnutls::x509::privkey::create());
//!
//! pk->generate(GNUTLS_PK_RSA, GNUTLS_SEC_PARAM_NORMAL);
//! pk->fix();
//! \endcode
//!
//! A new private key is created by instantiating a new
//! \ref privkey "INSERT_LIBX_NAMESPACE::gnutls::x509::privkey"
//! object then invoking the object's \c generate() method, followed by the
//! \c fix() method which, according to GnuTLS's documentation, recalculates
//! the key's secondary parameters.
//!
//! \c generate() receives either a \c GNUTLS_PK_RSA or a \c GNUTLS_PK_RSA
//! parameter, that sets the key's type, and the security strength of
//! the resulting key.
//!
//! \note
//! \c generate() can take several minutes to gather the necessary entropy,
//! before producing a new private key.
//!
//! The security strength parameter is actually a
//! \ref sec_param "INSERT_LIBX_NAMESPACE::gnutls::sec_param" object, which
//! can also be specified as a string value, which can be "weak", "low",
//! "normal", "high", or "ultra".
//!
//! \note
//! By default, \c generate()
//! uses a quick random number generator that's sufficient for most
//! applications. In situations that require high quality entropy, the
//! application-wide %property \c INSERT_LIBX_NAMESPACE::gnutls::random should
//! be set to "high". This setting is effective at application startup, so
//! this %property must be set in the application %property file.
//! Note that high quality entropy typically results in very slow private key
//! generation, as the GnuTLS library waits for sufficient amount of high
//! quality entropy to become available.
//!
//! \see property
//!
//! \par Progress notification
//!
//! \code
//! INSERT_LIBX_NAMESPACE::gnutls::progress_notifier notifier;
//!
//! pk->generate(GNUTLS_PK_RSA, GNUTLS_SEC_PARAM_NORMAL);
//! \endcode
//!
//! Instantiate an
//! \ref progress_notifier "INSERT_LIBX_NAMESPACE::gnutls::progress_notifier"
//! object to provide regular feedback
//! while a private key is being generated. The default feedback implementation
//! writes characters to standard error. Custom feedback may be implemented
//! by subclassing
//! \ref progress_notifier "INSERT_LIBX_NAMESPACE::gnutls::progress_notifier"
//! and subclassing its progress()
//! method.
//!
//! \note
//! Only one instance of the progress notifier object may be in scope at any
//! given time, in a single application thread. Different application threads
//! may instantiate their own individual progress notification objects.
//!
//! \par Exporting and importing keys
//!
//! \code
//! INSERT_LIBX_NAMESPACE::gnutls::datum_t key=pk->export_pkcs1(GNUTLS_X509_FMT_PEM);
//!
//! // ...
//! INSERT_LIBX_NAMESPACE::gnutls::x509::privkey pk2(INSERT_LIBX_NAMESPACE::gnutls::x509::privkey::create());
//!
//! pk2->import_pkcs1(key, GNUTLS_X509_FMT_PEM);
//! \endcode
//!
//! Use the \c export_pkcs1() method to export the private key.
//! \c export_pkcs1() receives either a \c GNUTLS_X509_FMT_PEM or a
//! \c GNUTLS_X509_FMT_DER parameter.
//!
//! \see \link vector vector<class T, class Allocator=std::allocator<T> > \endlink
//!
//! A private key is imported by instantiating a new
//! \ref privkey "INSERT_LIBX_NAMESPACE::gnutls::x509::privkey",
//! then invoking the \c import_pkcs1() method, with
//! analogous parameters.
//!
//! \par Exporting and importing passphrase-protected keys
//!
//! \code
//! INSERT_LIBX_NAMESPACE::gnutls::datum_t key=pk->export_pkcs8(GNUTLS_X509_FMT_PEM, "password",
//!                              GNUTLS_PKCS8_USE_PKCS12_3DES);
//!
//! // ...
//! INSERT_LIBX_NAMESPACE::gnutls::x509::privkey pk2(gnutls::x509::privkey::create());
//!
//! pk2->import_pkcs8(key, GNUTLS_X509_FMT_PEM, "password",
//!                   GNUTLS_PKCS8_USE_PKCS12_3DES);
//! \endcode
//!
//! These functions import and export passphrase-protected private keys.
//! \c export_pkcs8() and \c import_pkcs8() take additional arguments:
//! the password, and the encryption function. At press time, the
//! available encryption functions are:
//! \c GNUTLS_PKCS8_USE_PKCS12_3DES,
//! \c GNUTLS_PKCS8_USE_PKCS12_ARCFOUR,
//! and \c GNUTLS_PKCS8_USE_PKCS12_RC2_40.
//!
//! \par Exporting and importing raw keys
//!
//! \code
//! INSERT_LIBX_NAMESPACE::gnutls::privkey rawkey=pk->export_raw();
//!
//! // ...
//!
//! INSERT_LIBX_NAMESPACE::gnutls::x509::privkey pk2(rawkey->import_raw());
//!
//! pk2->import_raw(rawkey);
//! \endcode
//!
//! The \c export_raw() method returns a reference to an object that holds
//! raw private key algorithm data. import_raw() import the raw data into
//! an empty private key object.
//!
//! \code
//! switch (rawkey->get_pk_algorithm()) {
//! case GNUTLS_PK_RSA:
//!     rsakeyfunc(INSERT_LIBX_NAMESPACE::gnutls::rsaprivkey(rawkey));
//!     break;
//! case GNUTLS_PK_DSA:
//!     dsakeyfunc(INSERT_LIBX_NAMESPACE::gnutls::dsaprivkey(rawkey));
//!     break;
//! }
//! \endcode
//!
//! The raw key object's \c get_pk_algorithm() method returns either
//! \c GNUTLS_PK_RSA, and \c GNUTLS_PK_DSA. This governs whether the reference
//! can be safely casted to a
//! \ref rsaprivkey "INSERT_LIBX_NAMESPACE::gnutls::rsaprivkey"
//! or a
//! \ref dsaprivkey "INSERT_LIBX_NAMESPACE::gnutls::dsaprivkey"
//! (which in turn can be casted to a
//! \ref rsapubkey "INSERT_LIBX_NAMESPACE::gnutls::rsapubkey"
//! or a
//! \ref dsapubkey "INSERT_LIBX_NAMESPACE::gnutls::dsapubkey").
//!
//! When the private key's type is known, dedicated varieties of
//! \c export_raw() may be used:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::gnutls::rsaprivkey rsakey=pk->export_rsa_raw();
//!
//! // ...
//!
//! INSERT_LIBX_NAMESPACE::gnutls::dsaprivkey dsakey=pk->export_dsa_raw();
//! \endcode
//!
//! \c import_raw() is the opposite of export_raw, it takes a raw private key
//! and turns it into a
//! \ref privkey "INSERT_LIBX_NAMESPACE::gnutls::x509::privkey".
//! import_raw() is defined in
//! the
//! \ref gnutls::privkey "INSERT_LIBX_NAMESPACE::gnutls::privkey"
//! superclass, and implemented in its
//! \ref gnutls::rsaprivkey "INSERT_LIBX_NAMESPACE::gnutls::rsaprivkey"
//! and
//! \ref gnutls::dsaprivkey "INSERT_LIBX_NAMESPACE::gnutls::dsaprivkey"
//! subclasses.
//!
//! \par Retrieving the key's fingerprint
//!
//! \code
//! INSERT_LIBX_NAMESPACE::gnutls::datum_t fingerprint=pk->get_key_id();
//! \endcode

typedef ref<privkeyObj, privkeyBase> privkey;

//! A reference to a constant private key

//! \see #privkey
//!

typedef const_ref<privkeyObj, privkeyBase> const_privkey;

//! A nullable pointer reference to a private key

//! \see #privkey
//!
typedef ptr<privkeyObj, privkeyBase> privkeyptr;

//! A nullable pointer reference to a constant private key

//! \see #privkey
//!
typedef const_ptr<privkeyObj, privkeyBase> const_privkeyptr;

#if 0
{
#endif
}

#endif
