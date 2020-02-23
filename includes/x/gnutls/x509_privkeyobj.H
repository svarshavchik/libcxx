/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_x509_privkeyobj_H
#define x_tls_x509_privkeyobj_H

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {
		class rsaprivkeyobj;
		class dsaprivkeyobj;

		namespace x509 {
			class crtObj;
			class privkeyObj;
		};

		namespace credentials {
			class certificateObj;
		}
	}
}

#include <x/obj.H>
#include <x/ptrfwd.H>
#include <x/exception.H>
#include <x/vector.H>
#include <x/gnutls/init.H>
#include <x/gnutls/rsaprivkey.H>
#include <x/gnutls/dsaprivkey.H>
#include <x/gnutls/privkey.H>
#include <x/gnutls/pkparams.H>
#include <x/gnutls/sec_param.H>
#include <gnutls/x509.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! A wrapper for a \c gnutls_x509_privkey_t

//! See also:
//! \ref gnutls::x509::privkey "INSERT_LIBX_NAMESPACE::gnutls::x509::privkey".
//!
class gnutls::x509::privkeyObj : virtual public obj {

	//! The underlying GnuTLS handle

	gnutls_x509_privkey_t privkey;

public:
	friend class gnutls::x509::crtObj;
	friend class gnutls::credentials::certificateObj;
	friend class gnutls::rsaprivkeyobj;
	friend class gnutls::dsaprivkeyobj;

	//! The default constructor
	privkeyObj();

	//! The default destructor
	~privkeyObj();

	//! Generate a new private key

	void generate(//! \c GNUTLS_PK_DSA or \c GNUTLS_PK_RSA
		      gnutls_pk_algorithm_t algo,

		      //! Key strength
		      const sec_param &strength);

	//! Generate a new private key

	//! \overload
	//!
	void generate(//! Name of the public key algorithm
		      const std::string &algo,

		      //! Key strength
		      const sec_param &strength);

	//! Fix the key
	void fix();

	//! Export this key

	//! \return the key in \c PEM o \c DER format
	//!

	datum_t export_pkcs1(//! Output format, \c GNUTLS_X509_FMT_PEM or \c GNUTLS_X509_FMT_DER
			     gnutls_x509_crt_fmt_t format)
		const;

	//! Export this key to a file

	//! \overload
	//!
	void export_pkcs1(//! Filename
			  const std::string &filename,
			  //! Output format, \c GNUTLS_X509_FMT_PEM or \c GNUTLS_X509_FMT_DER
			  gnutls_x509_crt_fmt_t format)
		const;

	//! Export the key in PKCS8 format
	datum_t export_pkcs8(//! Output format, \c GNUTLS_X509_FMT_PEM or \c GNUTLS_X509_FMT_DER
			     gnutls_x509_crt_fmt_t format,

			     //! Passphrase
			     const std::string &password,

			     //! \c GNUTLS_PKCS8_PLAIN, \c GNUTLS_PKCS8_USE_PKCS12_3DES, \c GNUTLS_PKCS8_USE_PKCS12_ARCFOUR, or \c GNUTLS_PKCS8_USE_PKCS12_RC2_40

			     gnutls_pkcs_encrypt_flags_t flags)
		const;

	//! Import the key

	void import_pkcs1(//! Key data
			  datum_t keyref,

			  //! Input format, \c GNUTLS_X509_FMT_PEM or \c GNUTLS_X509_FMT_DER
			  gnutls_x509_crt_fmt_t format)
;

	//! Import the key

	void import_pkcs1(//! Filename
			  const std::string &filename,

			  //! Input format, \c GNUTLS_X509_FMT_PEM or \c GNUTLS_X509_FMT_DER
			  gnutls_x509_crt_fmt_t format)
;

	//! Import the key

	void import_pkcs1(//! Filename
			  const char *filename,

			  //! Input format, \c GNUTLS_X509_FMT_PEM or \c GNUTLS_X509_FMT_DER
			  gnutls_x509_crt_fmt_t format)

	{
		import_pkcs1(std::string(filename), format);
	}

	//! Import the PKCS8-formatted key

	void import_pkcs8(//! Key data
			  datum_t keyref,

			  //! Output format, \c GNUTLS_X509_FMT_PEM or \c GNUTLS_X509_FMT_DER
			  gnutls_x509_crt_fmt_t format,

			  //! Passphrase
			  const std::string &password,

			  //! \c GNUTLS_PKCS8_PLAIN, \c GNUTLS_PKCS8_USE_PKCS12_3DES, \c GNUTLS_PKCS8_USE_PKCS12_ARCFOUR, or \c GNUTLS_PKCS8_USE_PKCS12_RC2_40

			  gnutls_pkcs_encrypt_flags_t flags)
;

	//! Return the private key ID

	//! \return An opaque hash of the private key
	//!
	gnutls::datum_t get_key_id() const;

	//! Return the private key's algorithm

	//! \return \c GNUTLS_PK_DSA or \c GNUTLS_PK_RSA
	//!

	gnutls_pk_algorithm_t get_pk_algorithm() const;

	//! Return an object for temporary key exchange parameters

	//! This method instantiates a new pkparams object for this private
	//! key.
	//!
	//! \return a reference to a new pkparams object. The temporary
	//! key exchange parameters have not actually been created. For that,
	//! use the object's
	//! \link pkparamsObj::generate() generate()\endlink or
	//! \link pkparamsObj::import_pk() import_pk()\endlink method.

	pkparams get_pkparams() const;

	//! Export private key

	gnutls::privkey export_raw() const;

	//! Import private key

	void import_raw(gnutls::privkey privkey);

	//! Export raw RSA private key

	gnutls::rsaprivkey export_rsa_raw() const;

	//! Import raw RSA private key

	void import_rsa_raw(gnutls::rsaprivkey privkey);

	//! Export raw DSA private key

	gnutls::dsaprivkey export_dsa_raw() const;

	//! Import raw DSA private key

	void import_dsa_raw(gnutls::dsaprivkey privkey);
};

#if 0
{
#endif
}

#endif
