/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_credentials_certificateobj_H
#define x_tls_credentials_certificateobj_H

#include <x/namespace.h>
#include <x/ptrfwd.H>
#include <x/obj.H>
#include <x/exception.H>
#include <x/property_value.H>
#include <x/gnutls/init.H>
#include <x/gnutls/x509_privkey.H>
#include <x/gnutls/x509_crt.H>
#include <x/gnutls/dhparams.H>

#include <gnutls/gnutls.h>

#include <list>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {

		class sessionObj;

		namespace credentials {

			extern "C" {
				//! Callback function -- retrieve temporary parameters

				//! \internal
				//!
				int get_params_cb(gnutls_session_t session,
						  gnutls_params_type_t type,
						  gnutls_params_st *st);
			};

			class certificateObj;
			class callbackObj;
		}
	}
#if 0
};
#endif

//! A wrapper for a \c gnutls_certificate_credentials_t

class gnutls::credentials::certificateObj : virtual public obj {

	//! Private keys that are installed into the certificate object

	//! gnutls 2.12 does not copy the private key that's given to
	//! gnutls_certificate_set_x509_key(), so we must keep a reference to
	//! it.

	std::list<gnutls::x509::privkey> privatekeyList;

	//! The certificate credentials handle

	gnutls_certificate_credentials_t cred;

	//! Reference to the certificate's DH parameters

	dhparamsptr dh;

	//! A %credentials callback for this certicicate

	ptr<credentials::callbackObj> certificateCallback;

public:
	friend class gnutls::sessionObj;
	friend class gnutls::credentials::callbackObj;

	//! Default create() method

	//! Create an uninitialized credentials object.
	//!

	certificateObj();

	//! Create a credentials object from a certificate and a key file

	//! This create() method initializes a credentials object for a stock
	//! server. The credentials object gets initialized with all the default
	//! parameters, and the indicated certificate and key file gets loaded
	//! into the credentials object.
	//!
	//! Depending on the private key, an ephemeral DH key generator, and
	//! a temporary RSA parameter generator, configured with their default
	//! settings may be instantiated to provided EDH and temporary RSA
	//! parameters for sessions that use this credentials object.

	certificateObj(//! Certificate
		       const std::string &certfile,

		       //! Private key
		       const std::string &keyfile,

		       //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
		       gnutls_x509_crt_fmt_t format);

	//! Create a credentials object from a certificate and a key file

	//! \overload
	//!
	certificateObj(//! Certificate
		       const std::list<gnutls::x509::crt> &certlist,

		       //! Private key
		       const gnutls::x509::privkey &keyfile);

private:
	//! Internal initialization method

	void init() LIBCXX_INTERNAL;

public:
	//! Default destructor
	~certificateObj() noexcept;

	//! Set the certificate and key for this credentials object

	void set_key(//! Certificate path
		     const std::list<gnutls::x509::crt> &certlistArg,

		     //! Private key for the last certificate in the path
		     const gnutls::x509::privkey &certkeyArg);

	//! Load the certificate and private key from files

	void set_x509_keyfile(//! Certificate file
			      const std::string &certfile,

			      //! Private key file
			      const std::string &keyfile,

			      //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
			      gnutls_x509_crt_fmt_t format);

	//! Load the certificate and private key from buffers

	void set_x509_key_mem(//! Certificate path
			      gnutls::datum_t cert,

			      //! Private key
			      gnutls::datum_t key,

			      //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
			      gnutls_x509_crt_fmt_t format);

	//! Set the list of trusted X.509 certificate authorities

	void set_x509_trust(//! Trusted X.509 certificate authorities
			    const std::list<gnutls::x509::crt> &caList)
;

	//! Set the list of trusted X.509 certificate authorities

	//! \overload
	//!

	void set_x509_trust(//! Certificate authorities
			    gnutls::datum_t datum,

			    //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
			    gnutls_x509_crt_fmt_t format);

	//! Set the list of trusted X.509 certificate authorities from a file

	void set_x509_trust_file(//! Filename
				 std::string filename,
				 //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
				 gnutls_x509_crt_fmt_t format);

	//! Load the default list of trusted X.509 certificat authorities

	//! The certificates are loaded from the file specified by the
	//! \c INSERT_LIBX_NAMESPACE::gnutls::calist property.

	void set_x509_trust_default();

private:

	//! The \c INSERT_LIBX_NAMESPACE::gnutls::calist property.

	static property::value<std::string> calist;

public:
	//! Set verification flags

	void set_verify_flags(//! \c GNUTLS_VERIFY_* flag bitmask
			      unsigned int flags);


	//! Set verification limits

	void set_verify_limits(//! Max # of bits
			       unsigned int   	max_bits,
			       //! Max chain depths
			       unsigned int   	max_depth);

	//! Set the DH parameters to use for key exchange

	void set_dh_params(//! DH parameters
			   const dhparams &dhArg);

	//! Set the DH or the RSA parameters to use for the key exchange

	//! This function calls either set_dh_params() or set_rsa_params(),
	//! depending on the public key parameter type.

	void set_pk_params(//! Public key parameters
			   const pkparams &pkArg);

	//! Define a %credentials callback retrieval function

	void set_callback(//! The callback object
			  const ref<credentials::callbackObj> &callbackArg);

	//! Retrieve temporary parameters

	//! \internal
	//!

	static int get_params_cb(gnutls_session_t session,
				 gnutls_params_type_t type,
				 gnutls_params_st *st) noexcept;

private:
	//! Retrieve temporary parameters

	//! \internal
	//!
	int get_params(gnutls_params_type_t type,
		       gnutls_params_st *st)
;
};

#if 0
{
#endif
}
#endif
