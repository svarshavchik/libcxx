/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_pubkeyobj_H
#define x_tls_pubkeyobj_H

#include <x/namespace.h>
#include <x/obj.H>

#include <gnutls/gnutls.h>

namespace LIBCXX_NAMESPACE::gnutls {
	class pubkeyobj;
#if 0
}
#endif

//! An internal representation of a public key

//! This is a reference-counted superclass that holds public keys in GnuTLS's
//! internal form.

class pubkeyobj : virtual public obj {

public:
	//! Default constructor

	pubkeyobj();

	//! Default destructor
	~pubkeyobj();

	//! Return this public key's algorithm

	//! \return \c GNUTLS_PK_RSA or \c GNUTLS_PK_DSA
	//!
	//! If \c GNUTLS_PK_RSA: this object can be dynamic-casted to
	//! gnutls::rsapubkeyobj.
	//!
	//! If \c GNUTLS_PK_DSA: this object can be dynamic-casted to
	//! gnutls::dsapubkeyobj.

	virtual gnutls_pk_algorithm_t get_pk_algorithm()
		const=0;
};

#if 0
{
#endif
}

#endif
