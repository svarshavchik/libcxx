/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_x509_crtobjfwd_H
#define x_tls_x509_crtobjfwd_H

#include <x/namespace.h>
#include <x/gnutls/datumwrapper.H>
#include <x/gnutls/x509_crtfwd.H>
#include <x/ptrfwd.H>

#include <list>

namespace LIBCXX_NAMESPACE::gnutls::x509 {
	class crtObj;

	class crtBase;
}

namespace LIBCXX_NAMESPACE::gnutls::credentials {
	class certificateObj;
}

namespace LIBCXX_NAMESPACE {
#if 0
}
#endif

//! Base class for \ref crt "INSERT_LIBX_NAMESPACE::gnutls::x509::crt".

//! This base class defined several static member functions of
//! \ref crt "INSERT_LIBX_NAMESPACE::gnutls::x509::crt".
//!
//! Refer to this class as
//! \c INSERT_LIBX_NAMESPACE::gnutls::x509::crt::base
//! or
//! \c INSERT_LIBX_NAMESPACE::gnutls::x509::crtptr::base.

class gnutls::x509::crtBase : public ptrref_base {

public:
	//! Import a list of certificates

	static void import_cert_list( //! Certificate objects placed here
				     std::list<crt> &certList,

				     //! The certificates

				     gnutls::datum_t certs,

				     //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
				     gnutls_x509_crt_fmt_t format)
;

	//! Import a list of certificates

	//! \overload
	static void import_cert_list( //! Certificate objects placed here
				     std::list<crt> &certList,

				     //! The certificates

				     const gnutls::datumwrapper &certs,

				     //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
				     gnutls_x509_crt_fmt_t format)
;

	//! Import a list of certificates

	//! \overload
	//!
	static void import_cert_list( //! Certificate objects placed here
				     std::list<crt> &certList,

				     //! The certificates

				     fd certfd,

				     //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
				     gnutls_x509_crt_fmt_t format)
;

	//! Import a list of certificates

	//! \overload
	//!
	static void import_cert_list( //! Certificate objects placed here
				     std::list<crt> &certList,

				     //! The certificate file

				     const std::string &filename,

				     //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
				     gnutls_x509_crt_fmt_t format)
;

	//! Import a list of certificates

	//! \overload
	//!
	static void import_cert_list( //! Certificate objects placed here
				     std::list<crt> &certList,

				     //! The certificate file

				     const char *filename,

				     //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
				     gnutls_x509_crt_fmt_t format)
;

	//! Verify certificate chain

	//! \return 0 if the certificate chain has been verified, else a bitmask
	//! of \c GNUTLS_CERT_INVALID and \c GNUTLS_CERT_REVOKED.

	template<typename cert_chain_container_t,
		 typename ca_list_container_t>
	static unsigned int verify_cert_list(//! Certificates to verify
					     const cert_chain_container_t
					     &certList,

					     //! Certificate authorities
					     const ca_list_container_t
					     &caList,

					     //! \c GNUTLS_VERIFY_* flag bitmask

					     unsigned int flags=0)
;

private:

	//! Common certificate chain verification code

	//! \internal
	//!
	static unsigned int
	verify_cert_list_internal(//! Certificate chain
				  std::vector<gnutls_x509_crt_t> &certList,

				  //! Certificate authorities
				  std::vector<gnutls_x509_crt_t> &caList,

				  //! Verification flags
				  unsigned int flags);
};

#if 0
{
#endif
}
#endif
