/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_dsaprivkey_H
#define x_tls_dsaprivkey_H

#include <x/namespace.h>
#include <x/gnutls/dsaprivkeyobj.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {

		//! A DSA private key

		//! This object holds an DSA private key, in raw form.
		//! This is a reference to a reference-counted object.
		//! After this reference is returned from a method, it can
		//! be freely passed around.
		typedef ref<dsaprivkeyobj> dsaprivkey;

		//! A reference to a constant DSA private key

		//! \see #dsaprivkey
		//!
		typedef const_ref<dsaprivkeyobj> const_dsaprivkey;


		//! A nullable pointer reference to a DSA private key
		//! \see #dsaprivkey
		//!

		typedef ptr<dsaprivkeyobj> dsaprivkeyptr;

		//! A nullable pointer reference to a constant DSA private key

		//! \see #dsaprivkey
		//!
		typedef const_ptr<dsaprivkeyobj> const_dsaprivkeyptr;

	}
}
#endif
