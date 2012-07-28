/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_rsaparams_H
#define x_tls_rsaparams_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/gnutls/rsaparamsobj.H>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {
#if 0
	};
};
#endif

//! Temporary RSA parameters for TLS/SSL sessions

//! \code
//! gnutls::rsaparams rsap(gnutls::rsaparams::create());
//!
//! rsap->generate(512);
//! \endcode
//!
//! Temporary RSA parameters are needed for TLS/SSL session that use
//! RSA-based encryption.
//! \link gnutls::rsaparamsObj::generate() generate() \endlink generates
//! new RSA parameters.
//!
//! \note
//! By default, \link gnutls::rsaparamsObj::generate() generate() \endlink
//! uses a strong cryptographic entropy pool. Generating even a small RSA
//! key may require a lengthy period of time before sufficient entropy is
//! generate. Before initializing the library, invoke
//! gnutls::init::quick_random() to use a faster, but less secure entropy pool.
//! Note that this is a global %option that affects all functions that consume
//! the random entropy pool.
//!
//! \code
//! rsap->export_pk(GNUTLS_X509_FMT_PEM)->save("rsaparams.dat");
//!
//! // ...
//!
//! gnutls::datum_t rsa_dat(gnutls::datum_t::create());
//!
//! rsa_data->load("rsaparams.dat");
//!
//! rsap->import_pk(rsa_dat, GNUTLS_X509_FMT_PEM);
//! \endcode
//!
//! generate() typically takes a long time. The usual approach is to
//! generate temporary RSA parameters in advance, and use
//! \link gnutls::rsaparamsObj::export_pk() export_pk() \endlink to
//! save them in a file. Subsequently, the
//! \link gnutls::rsaparamsObj::import_pk() import_pk() \endlink
//! function quickly retrieves the RSA parameters.
//!
//! Suggested good practice is to use
//! \ref rsaparamsObj::import() "import()" to load the system-generated global
//! temporary RSA parameters:
//!
//! \code
//! gnutls::rsaparams rsap(gnutls::rsaparams::create());
//!
//! rsap->import()
//! \endcode

typedef ptr<rsaparamsObj> rsaparams;

#if 0
{
	{
#endif
	}
}
#endif
