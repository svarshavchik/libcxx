/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_dsapubkeyobj_H
#define x_tls_dsapubkeyobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/gnutls/datumwrapper.H>
#include <x/gnutls/pubkeyobj.H>

namespace LIBCXX_NAMESPACE::gnutls {
	class dsapubkeyobj;
#if 0
}
#endif

//! An internal representation of a DSA public key

//! This reference-counted object holds the DSA public key in GnuTLS's
//! internal form. A reference to this object is returned by a wrapper for
//! some GnuTLS function.

class dsapubkeyobj : virtual public pubkeyobj, virtual public obj {

public:
	//! Default constructor
	dsapubkeyobj();

	//! Default destructor
	~dsapubkeyobj();

	datum_t p,		//!< The \c p value
		q,		//!< The \c q value
		g,		//!< The \c g value
		y;		//!< The \c y value

	//! Return algorithm type

	//! \return \c GNUTLS_PK_DSA

	gnutls_pk_algorithm_t get_pk_algorithm() const override;
};

#if 0
{
#endif
}
#endif
