/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_dsaprivkeyobj_H
#define x_tls_dsaprivkeyobj_H

#include <x/namespace.h>
#include <x/gnutls/dsapubkeyobj.H>
#include <x/gnutls/x509_privkey.H>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {
		class dsaprivkeyobj;
	}
}

#include <x/gnutls/x509_privkeyobj.H>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {

#if 0
	};
};
#endif

//! An internal representation of a DSA private key

//! This reference-counted object holds the DSA private key in GnuTLS's
//! internal form. A reference to this object is returned by a wrapper for
//! some GnuTLS function.
//!
//! The private key is a subclass of a public key.

class dsaprivkeyobj : public dsapubkeyobj, virtual public privkeyobj {

public:
	//! Default constructor
	dsaprivkeyobj();

	//! Default destructor
	~dsaprivkeyobj();

	datum_t x;	//!< The \c x value

	x509::privkey import_raw() const override;
};

#if 0
{
	{
#endif
	}
}
#endif
