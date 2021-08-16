/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_privkeyobj_H
#define x_tls_privkeyobj_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/gnutls/pubkeyobj.H>
#include <x/gnutls/x509_privkeyfwd.H>

namespace LIBCXX_NAMESPACE::gnutls {
#if 0
}
#endif

//! An internal representation of a private key

//! This is a reference-counted superclass that holds private keys in GnuTLS's
//! internal form.

class privkeyobj : virtual public obj, virtual public pubkeyobj {

public:
	//! Default constructor

	privkeyobj() noexcept;

	//! Default destructor
	~privkeyobj();

	//! Import this raw private key into a gnutls::x509::privkey object

	virtual x509::privkey import_raw() const=0;
};

#if 0
{
#endif
}
#endif
