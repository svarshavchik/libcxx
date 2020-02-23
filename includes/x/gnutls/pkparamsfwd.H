/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_pkparamsfwd_H
#define x_tls_pkparamsfwd_H

#include <x/namespace.h>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {
#if 0
	};
};
#endif

class pkparamsObj;
class pkparamsBase;

//! Temporary public key parameters for TLS/SSL sessions

//! This is a reference to a generic superclass, currently subclassed only by
//! dhparamsObj. This reference gets returned by
//! \link x509::privkeyObj::get_pkparams() x509::privkey-&gt;get_pkparams()\endlink.
//!
//! \par Manually creating public key parameters
//!
//! \code
//! gnutls::pkparams pk= gnutls::pkparams::create("DSA");
//! \endcode
//!
//! Without a convenient \c x509::privkey object, a pkparams may be created
//! manually by invoking the
//! \ref pkparamsBase::newobj::create(const std::string &) "create()"
//! function. create() takes a public key type, currently the only valid
//! public key type is "DSA". An overloaded
//! \ref pkparamsBase::newobj::create(gnutls_pk_algorithm_t) "create()"
//! function also takes \c GNUTLS_PK_DSA.
//!
//! \par Exporting and importing public key parameters
//!
//! \code
//! gnutls::datum_t d=pk->export_pk(GNUTLS_X509_FMT_PEM);
//! \endcode
//!
//! \link pkparamsObj::export_pk() export_pk() \endlink exports the public
//! key parameters to a \c GNUTLS_X509_FMT_PEM or a \c GNUTLS_X509_FMT_DER
//! format.
//!
//! \code
//! pk->import_pk(d, GNUTLS_X509_FMT_PEM);
//! \endcode
//!
//! \link pkparamsObj::import_pk() import_pk() \endlink does the opposite.
//!
//! \code
//! std::vector<datum_t> factors;
//! unsigned int nbits;
//!
//! pk->export_raw(factors, nbits);
//!
//! pk->import_raw(factors);
//! \endcode
//!
//! \link pkparamsObj::export_raw() export_raw() \endlink exports the raw
//! public key parameter values, and
//! \link pkparamsObj::import_raw() import_raw() \endlink imports them back in.
//!
//! \code
//! unsigned int bits=pk->get_bits();
//!
//! gnutls_pk_algorithm_t algo=pk->get_pk_algorithm();
//! \endcode
//!
//! \link pkparamsObj::get_bits() get_bits() \endlink is a shortcut
//! for retrieving the number of
//! bits in the public key parameters (but not their values).
//! \link pkparamsObj::get_pk_algorithm() get_pk_algorithm() \endlink
//! returns \c GNUTLS_PK_RSA or \c GNUTLS_PK_DSA to indicate the underlying
//! public key parameter types (RSA or DH).

typedef ref<pkparamsObj, pkparamsBase> pkparams;

//! A nullable pointer reference to a public key parameter object

//! \see pkparams
typedef ptr<pkparamsObj, pkparamsBase> pkparamsptr;

#if 0
{
	{
#endif
	}
}
#endif
