/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_x509_crtfwd_H
#define x_tls_x509_crtfwd_H

#include <x/namespace.h>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {

		namespace x509 {
#if 0
		};
	};
};
#endif

class crtObj;
class crtBase;

#if 0
{
	{
#endif
	}
}

namespace gnutls {

	namespace x509 {
#if 0
		};
	};
#endif

//! An X509 certificate

//! This is a reference to an X509 certificate object.
//!
//! \par Creating a new certificate
//!
//! Use a
//! \ref privkey "INSERT_LIBX_NAMESPACE::gnutls::x509::privkey"
//! object to create a new certificate key.
//! First, create a new certificate object:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::gnutls::x509::crt cacert(INSERT_LIBX_NAMESPACE::gnutls::x509::crt::create());
//! \endcode
//!
//! Set the certificate's activation, expiration time, and name:
//!
//! \code
//! cacert->set_activation_time(current_time);
//! cacert->set_expiration_time(current_time + 60 * 60 * 24 * 400L);
//! cacert->set_dn_by_oid(GNUTLS_OID_X520_ORGANIZATION_NAME,
//!                       "libx library");
//! cacert->set_dn_by_oid(GNUTLS_OID_X520_ORGANIZATIONAL_UNIT_NAME,
//!                       "GnuTLS wrapper");
//! cacert->set_dn_by_oid(GNUTLS_OID_X520_COMMON_NAME,
//!                       "example.com");
//! cacert->set_subject_alternative_name(GNUTLS_SAN_DNSNAME,
//!                                      "domain.com");
//! \endcode
//!
//! See also:
//! \ref crtObj::set_activation_time "set_activation_time"(),
//! \ref crtObj::set_expiration_time "set_expiration_time"(),
//! \ref crtObj::set_dn_by_oid "set_dn_by_oid"(), and
//! \ref crtObj::set_subject_alternative_name "set_subject_alternative_name"().
//!
//! Set the certificate's type:
//!
//! \code
//! cacert->set_key_usage(GNUTLS_KEY_DIGITAL_SIGNATURE|
//!                       GNUTLS_KEY_NON_REPUDIATION|
//!                       GNUTLS_KEY_KEY_AGREEMENT);
//! cacert->set_key_purpose_oid(GNUTLS_KP_TLS_WWW_SERVER, false);
//! cacert->set_key_purpose_oid(GNUTLS_KP_CODE_SIGNING, false);
//! cacert->set_key_purpose_oid(GNUTLS_KP_EMAIL_PROTECTION, false);
//! \endcode
//!
//! See also:
//! \ref crtObj::set_key_usage "set_key_usage"() and
//! \ref crtObj::set_key_purpose_oid "set_key_purpose_oid"().
//!
//! If the new certificate is a certificate authority's certificate, mark it
//! accordingly, using
//! \ref crtObj::set_basic_constraints "set_basic_constraints"(),
//! and set its serial number to 1:
//!
//! \code
//! newcert->set_basic_constraints(true);
//! newcert->set_serial(1UL);
//! \endcode
//!
//! \note
//! An authority's certificate's key usage should include
//! \c GNUTLS_KEY_KEY_CERT_SIGN and \c GNUTLS_KEY_CRL_SIGN.
//!
//! For a regular, non-authority certificate, its
//! certificate authority needs to keep track of serial numbers, somehow
//! and set the new certificat's serial number accordingly:
//! \code
//! cacert->set_basic_constraints(false);
//! cacert->set_serial(serial_num);
//! \endcode
//!
//! See also:
//! \ref crtObj::set_basic_constraints "set_basic_constraints"() and
//! \ref crtObj::set_serial "set_serial"().
//!
//! Finalize the certificate by setting some required parameters:
//!
//! \code
//! cert->set_subject_key_id();
//! cert->set_version();
//! \endcode
//!
//! See also:
//! \ref crtObj::set_subject_key_id "set_subject_key_id"() and
//! \ref crtObj::set_version "set_version"().
//!
//! The final step involves creating the certificate's private key, and
//! signing the certificate. Certificate authority keys sign themselves:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::gnutls::x509::privkey cakey(INSERT_LIBX_NAMESPACE::gnutls::x509::privkey::create());
//!
//! cakey->generate(GNUTLS_PK_RSA, "normal");
//! cacert->set_key(cakey);
//! cacert->sign(cacert, cakey);
//! \endcode
//!
//! A non-authority's certificate is signed by a certificate authority's
//! certificate and the certificate authority's private key:
//! \code
//! INSERT_LIBX_NAMESPACE::gnutls::x509::privkey certkey(INSERT_LIBX_NAMESPACE::gnutls::x509::privkey::create());
//!
//! certkey->generate(GNUTLS_PK_RSA, "normal");
//! newcert->set_key(certkey);
//! newcert->sign(cacert, cakey);
//! \endcode
//!
//! The third optional parameter to sign() is the signature algorithm,
//! which defaults to \c GNUTLS_DIG_SHA256. The signature algorithm is actually
//! a \ref digest_algorithm "digest_algoritm" object that can be initialized by
//! a literal \c GNUTLS_DIG or by a string like "sha1".
//!
//! See also:
//! \ref privkey "INSERT_LIBX_NAMESPACE::gnutls::x509::privkey",
//! \ref crtObj::set_key "set_key"() and
//! \ref crtObj::sign "sign"().
//!
//! \par Exporting the new certificate
//!
//! A new certificate, and its private key, should be saved to a file:
//!
//! \code
//! newcert->export_cert(GNUTLS_X509_FMT_PEM)->save(pemfilename);
//! certkey->export_pkcs1(GNUTLS_X509_FMT_PEM)->save(crtfilename);
//! \endcode
//!
//! A little more work is required for a certificate that is cross-signed
//! by an intermediate certificate authority. With most applications, the
//! intermediate certificate authority's certificate should be saved in the
//! same certificate file first, before the new certificate.
//!
//! Correctly creating the certificate file is a little bit more complicated:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::fd hcertfile(INSERT_LIBX_NAMESPACE::fd::create());
//!
//! hcertfile->open(crtfilename, O_WRONLY | O_CREAT | O_TRUNC);
//!
//! icacert->export_cert(GNUTLS_X509_FMT_PEM)->save(hcertfile); // Intermediate CA certificate
//! newcert->export_cert(GNUTLS_X509_FMT_PEM)->save(hcertfile); // New certificate
//!
//! hcertfile->close();
//! \endcode
//!
//! See also:
//! \ref crtObj::export_cert "export_cert"().
//!
//! \par Importing certificates
//!
//! Because a certificate file may contain a certificate chain, with
//! a cross-signed intermediate certificate, the certificate files
//! are loaded into a list:
//!
//! \code
//! std::list<gnutls::x509::crt> certlist;
//!
//! gnutls::x509::crt::import_cert_list(certlist, certfilename,
//!                                     GNUTLS_X509_FMT_PEM);
//! \endcode
//!
//! See also:
//! \ref crtBase::import_cert_list() "INSERT_LIBX_NAMESPACE::gnutls::x509::crt::base::import_cert_list"().
//!
//! \note
//! Make sure that the \c certlist is non-empty!
//!
//! \par Validating certificates
//!
//! \code
//!
//! std::list<gnutls::x509::crt> certlist, calist;
//!
//! // ...
//!
//! if (gnutls::x509::crt::verify_cert_list(certlist, calist) != 0 ||
//!     !certlist.back()->check_hostname("example.com"))
//!     throw EXCEPTION("Certificate failed to verify");
//! \endcode
//!
//! The
//! \ref crtBase::verify_cert_list() "INSERT_LIBX_NAMESPACE::gnutls::x509::crt::base::verify_cert_list"()
//! function validates that the certificate list is signed by a trusted
//! certificate authority.
//! \ref crtObj::check_hostname "check_hostname"()
//! checks that the certificate chain's last certificate's subject's common name
//! matches the expected hostname.

typedef ptr<crtObj, crtBase> crt;

//! A reference to an immutable certificate

//! \see crt
//!

typedef const_ptr<crtObj, crtBase> const_crt;

//! A nullable pointer to a certificate

//! \see crt
//!

typedef ptr<crtObj, crtBase> crtptr;

//! A nullable pointer to an immutable certificate

//! \see crt
//!
typedef const_ptr<crtObj, crtBase> const_crtptr;


#if 0
{
	{
		{
#endif
		}
	}
}
#endif
