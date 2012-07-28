/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_rsaprivkey_H
#define x_tls_rsaprivkey_H

#include <x/namespace.h>
#include <x/gnutls/rsaprivkeyobj.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {

		//! An RSA private key

		//! This object holds an RSA private key, in raw form.
		//! This is a reference to a reference-counted object.
		//! After this reference is returned from a method, it can
		//! be freely passed around.
		typedef ptr<rsaprivkeyobj> rsaprivkey;

		//! A reference to a constant RSA private key

		//! \see #rsaprivkey
		//!
		typedef const_ptr<rsaprivkeyobj> const_rsaprivkey;
	}
}

#endif
