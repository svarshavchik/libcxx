/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_rsapubkey_H
#define x_tls_rsapubkey_H

#include <x/namespace.h>
#include <x/gnutls/rsapubkeyobj.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE::gnutls {

	//! An RSA public key

	//! This object holds an RSA public key, in raw form.
	//! This is a reference to a reference-counted object.
	//! After this reference is returned from a method, it can
	//! be freely passed around.
	typedef ref<rsapubkeyobj> rsapubkey;

	//! A reference to a constant RSA public key

	//! \see #rsapubkey
	//!
	typedef const_ref<rsapubkeyobj> const_rsapubkey;

	//! A nullable reference to an RSA public key

	//! \see #rsapubkey
	//!
	typedef ptr<rsapubkeyobj> rsapubkeyptr;

	//! A nullable pointer reference to a constant RSA public key

	//! \see #rsapubkey
	//!
	typedef const_ptr<rsapubkeyobj> const_rsapubkeyptr;
}

#endif
