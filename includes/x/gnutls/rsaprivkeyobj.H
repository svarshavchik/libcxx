/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_rsaprivkeyobj_H
#define x_tls_rsaprivkeyobj_H

#include <x/namespace.h>
#include <x/gnutls/rsapubkeyobj.H>
#include <x/gnutls/privkeyobj.H>
#include <x/gnutls/x509_privkeyfwd.H>

namespace LIBCXX_NAMESPACE::gnutls {
	class rsaprivkeyobj;
}

#include <x/gnutls/x509_privkeyobj.H>

namespace LIBCXX_NAMESPACE::gnutls {
#if 0
}
#endif

//! An internal representation of an RSA private key

//! This reference-counted object holds the RSA private key in GnuTLS's
//! internal form. A reference to this object is returned by a wrapper for
//! some GnuTLS function.
//!
//! The private key is a subclass of a public key.

class rsaprivkeyobj : public rsapubkeyobj, virtual public privkeyobj {

public:
	//! Default constructor
	rsaprivkeyobj();

	//! Default destructor
	~rsaprivkeyobj();

	datum_t d,		//!< The \c d value
		p,		//!< The \c p value
		q,		//!< The \c q value
		u;		//!< The \c u value

	x509::privkey import_raw() const override;
};

#if 0
{
#endif
}

#endif
