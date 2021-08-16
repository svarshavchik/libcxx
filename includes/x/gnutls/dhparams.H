/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_dhparams_H
#define x_tls_dhparams_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/gnutls/dhparamsobj.H>

namespace LIBCXX_NAMESPACE::gnutls {
#if 0
}
#endif

//! Temporary DH parameters for TLS/SSL sessions

//! \code
//! gnutls::dhparams dhp(gnutls::dhparams::create());
//!
//! dhp->generate(1024);
//! \endcode
//!
//! Temporary DH parameters are needed for TLS/SSL session that use
//! DH-based encryption.
//! \link gnutls::dhparamsObj::generate() generate() \endlink generates
//! new DH parameters.
//!
//! \note
//! By default, \link gnutls::dhparamsObj::generate() generate() \endlink
//! uses a strong cryptographic entropy pool. Generating even a small DH
//! key may require a lengthy period of time before sufficient entropy is
//! generate. Before initializing the library, invoke
//! gnutls::init::quick_random() to use a faster, but less secure entropy pool.
//! Note that this is a global %option that affects all functions that consume
//! the random entropy pool.
//!
//! \code
//! dhp->export_pk(GNUTLS_X509_FMT_PEM)->save("dhparams.dat");
//!
//! // ...
//!
//! gnutls::datum_t dh_dat(gnutls::datum_t::create());
//!
//! dh_data->load("dhparams.dat");
//!
//! dhp->import_pk(dh_dat, GNUTLS_X509_FMT_PEM);
//! \endcode
//!
//! generate() typically takes a long time. The usual approach is to
//! generate temporary DH parameters in advance, and use
//! \link gnutls::dhparamsObj::export_pk() export_pk() \endlink to
//! save them in a file. Subsequently, the
//! \link gnutls::dhparamsObj::import_pk() import_pk() \endlink
//! function quickly retrieves the DH parameters.
//!
//! Suggested good practice is to use
//! \ref dhparamsObj::import() "import()" to load the system-generated global
//! DH parameters:
//!
//! \code
//! gnutls::dhparams dhp(gnutls::dhparams::create());
//!
//! dhp->import()
//! \endcode

typedef ref<dhparamsObj> dhparams;

//! A nullable pointer reference to a temporary DH parameter object

typedef ptr<dhparamsObj> dhparamsptr;

#if 0
{
#endif
}
#endif
