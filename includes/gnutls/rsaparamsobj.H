/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_rsaparamsobj_H
#define x_tls_rsaparamsobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/gnutls/init.H>
#include <x/gnutls/datumwrapper.H>
#include <x/gnutls/pkparamsobj.H>

#include <gnutls/gnutls.h>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {
		namespace credentials {
			class certificateObj;
		}
		class rsaparamsObj;
		class pkparamsObj;
#if 0
	};
};
#endif

//! An internal representation of RSA parameters

//! This reference-counted object holds RSA parameters in GnuTLS's
//! internal form.

class rsaparamsObj : public pkparamsObj {

	//! Underlying GnuTLS \c gnutls_rsa_params_t handle

	gnutls_rsa_params_t rsa;

public:
	friend class gnutls::credentials::certificateObj;

	//! Create an empty RSA parameter object

	rsaparamsObj();

	//! Default destructor

	~rsaparamsObj() noexcept;

	//! Generate new RSA parameters

	void generate(//! Number of bits
		      unsigned int bits);

	//! Export the RSA parameters

	datum_t export_pk(//! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
			  gnutls_x509_crt_fmt_t format);

	//! Import the RSA parameters

	void import_pk(//! The RSA parameters
		       datum_t rsaparams,
		       //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
		       gnutls_x509_crt_fmt_t format);

	//! Export raw key exchange parameters

	//! This function resizes \c raw_datum to a six-element
	//! array, then places \c m, \c e, \c d, \c p, \c q, and \c u
	//! values into the array,
	//! and sets \c bits to the number of bits in the key.

	virtual void export_raw(//! Raw public key parameter
				std::vector<datum_t> &raw_datum,

				//! Number of bits in the public key parameter
				unsigned int &bits);

	//! Import raw key exchange parameters

	virtual void import_raw(//! Raw public key parameter

				//! This must be a six-element array
				//! (\c m, \c e, \c d, \c p, \c q, and \c u ).

				const std::vector<datum_t> &raw_datum)
;

	//! Return the public key type

	//! \return \c GNUTLS_PK_RSA
	//!

	gnutls_pk_algorithm_t get_pk_algorithm();

	//! Disown this RSA parameter

	//! Here's the \c gnutls_rsa_params_t that this object owns. It
	//! no longer owns it, the caller is responsible for deinitializing
	//! it. This object now owns an uninitialized \c gnutls_rsa_params_t.

	operator gnutls_rsa_params_t();

	//! GnuTLS public key algorithm ID

	static const gnutls_pk_algorithm pk_algorithm=GNUTLS_PK_RSA;

	//! Import the default public key parameter file.

	//! Search the directory specified by the
	//! INSERT_LIBX_NAMESPACE::gnutls::tlsparamsdir
	//! \ref property "application property"
	//! for a public
	//! key parameter file. The following files are searched, and the first
	//! one found gets imported. An %exception gets thrown if no files were
	//! found:
	//!
	//! - user.{USERNAME}.rsa, where {USERNAME} is returned by geteuid().
	//!
	//! - group.{GROUPNAME}.rsa, where {GROUPNMAME} is returned by
	//! getegid(), and
	//! any supplementary groups this process belongs to.
	//!
	//! - system.suffix

	void import();
};

#if 0
{
	{
#endif
	}
}

#endif
