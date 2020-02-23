/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_rsapubkey_H
#define x_tls_rsapubkey_H

#include <x/namespace.h>
#include <x/gnutls/rsapubkeyobj.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {

		//! An RSA public key

		//! This object holds an RSA public key, in raw form.
		//! This is a reference to a reference-counted object.
		//! After this reference is returned from a method, it can
		//! be freely passed around.
		typedef ptr<rsapubkeyobj> rsapubkey;

		//! A reference to a constant RSA public key

		//! \see #rsapubkey
		//!
		typedef const_ptr<rsapubkeyobj> const_rsapubkey;
	}
}

#endif
