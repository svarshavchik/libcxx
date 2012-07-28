/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_dhparamsobj_H
#define x_tls_dhparamsobj_H

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

		class dhparamsObj;
		class pkparamsObj;
#if 0
	};
};
#endif

//! An internal representation of DH parameters

//! This reference-counted object holds DH parameters in GnuTLS's
//! internal form.

class dhparamsObj : public pkparamsObj {

	//! Underlying GnuTLS \c gnutls_dh_params_t handle

	gnutls_dh_params_t dh;

public:
	friend class gnutls::credentials::certificateObj;

	//! Create an empty DH parameter object

	dhparamsObj();

	//! Default destructor

	~dhparamsObj() noexcept;

	//! Generate new DH parameters

	void generate(//! Number of bits, 768, 1024, 2048, 3072 or 4096.
		      unsigned int bits);

	//! Export the DH parameters

	datum_t export_pk(//! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
			  gnutls_x509_crt_fmt_t format);

	//! Import the DH parameters
	void import_pk(//! The DH parameters
		       datum_t dhparams,
		       //! \c GNUTLS_X509_FMT_DER or \c GNUTLS_X509_FMT_PEM
		       gnutls_x509_crt_fmt_t format);

	//! Export raw key exchange parameters

	//! This function resizes \c raw_datum to a two-element
	//! array, then places \c prime and \c generator into the array,
	//! and sets \c bits to the number of bits in the key.

	void export_raw(//! Raw public key parameter
			std::vector<datum_t> &raw_datum,

			//! Number of bits in the public key parameter
			unsigned int &bits);

	//! Import raw key exchange parameters

	void import_raw(//! Raw public key parameter

			//! This must be a two-element array
			//! (\c prime and \c generator values).

			const std::vector<datum_t> &raw_datum);

	//! Return the public key type

	//! \return \c GNUTLS_PK_DSA
	//!

	gnutls_pk_algorithm_t get_pk_algorithm();

	//! Disown this DH parameter

	//! Here's the \c gnutls_dh_params_t that this object owns. It
	//! no longer owns it, the caller is responsible for deinitializing
	//! it. This object now owns an uninitialized \c gnutls_dh_params_t.

	operator gnutls_dh_params_t();

	//! GnuTLS public key algorithm ID

	static const gnutls_pk_algorithm pk_algorithm=GNUTLS_PK_DSA;

	//! Import the default public key parameter file

	//! Search the directory specified by the
	//! INSERT_LIBX_NAMESPACE::gnutls::tlsparamsdir
	//! \ref property "application property"
	//! for a public
	//! key parameter file. The following files are searched, and the first
	//! one found gets imported. An %exception gets thrown if no files were
	//! found:
	//!
	//! - user.{USERNAME}.dh, where {USERNAME} is returned by geteuid()
	//!
	//! - group.{GROUPNAME}.dh, where {GROUPNAME} is returned by
	//! getegid(), and
	//! any supplementary groups this process belongs to
	//!
	//! - system.dh

	void import();
};

#if 0
{
	{
#endif
	}
}
#endif
