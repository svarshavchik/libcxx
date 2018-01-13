/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_rsapubkeyobj_H
#define x_tls_rsapubkeyobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/gnutls/datumwrapper.H>
#include <x/gnutls/pubkeyobj.H>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {
		class rsapubkeyobj;
#if 0
	};
};
#endif

//! An internal representation of an RSA public key

//! This reference-counted object holds the RSA public key in GnuTLS's
//! internal form. A reference to this object is returned by a wrapper for
//! some GnuTLS function.

class rsapubkeyobj : virtual public pubkeyobj, virtual public obj {

public:
	//! Default constructor
	rsapubkeyobj();

	//! Default destructor
	~rsapubkeyobj();

	datum_t m,		//!< Modulus
		e;		//!< Exponent

	//! Return algorithm type

	//! \return \c GNUTLS_PK_RSA

	gnutls_pk_algorithm_t get_pk_algorithm() const override;
};

#if 0
{
	{
#endif
	}
}

#endif
