/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_rsaprivkey_H
#define x_tls_rsaprivkey_H

#include <x/namespace.h>
#include <x/gnutls/rsaprivkeyobj.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE::gnutls {

	//! An RSA private key

	//! This object holds an RSA private key, in raw form.
	//! This is a reference to a reference-counted object.
	//! After this reference is returned from a method, it can
	//! be freely passed around.
	typedef ref<rsaprivkeyobj> rsaprivkey;

	//! A constant reference to a constant RSA private key

	//! \see #rsaprivkey
	//!
	typedef const_ref<rsaprivkeyobj> const_rsaprivkey;

	//! A nullable pointer reference to an RSA private key

	//! \see #rsaprivkey
	//!

	typedef ptr<rsaprivkeyobj> rsaprivkeyptr;

	//! A constant reference to a constant RSA private key

	//! \see #rsaprivkey
	//!
	typedef const_ptr<rsaprivkeyobj> const_rsaprivkeyptr;
}

#endif
