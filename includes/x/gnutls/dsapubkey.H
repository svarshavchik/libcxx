/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_dsapubkey_H
#define x_tls_dsapubkey_H

#include <x/namespace.h>
#include <x/gnutls/dsapubkeyobj.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {

		//! A DSA public key

		//! This object holds a DSA public key, in raw form.
		//! This is a reference to a reference-counted object.
		//! After this reference is returned from a method, it can
		//! be freely passed around.
		typedef ref<dsapubkeyobj> dsapubkey;

		//! A reference to a constant DSA public key

		//! \see #dsapubkey
		//!
		typedef const_ref<dsapubkeyobj> const_dsapubkey;

		//! A nullable pointer reference to a DSA public key

		//! \see #dsapubkey
		//!

		typedef ptr<dsapubkeyobj> dsapubkeyptr;

		//! A nullable pointer reference to a constant DSA public key

		//! \see #dsapubkey
		//!
		typedef const_ptr<dsapubkeyobj> const_dsapubkeyptr;
	}
}
#endif
