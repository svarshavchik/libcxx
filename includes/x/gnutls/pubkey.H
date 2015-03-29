/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_pubkey_H
#define x_tls_pubkey_H

#include <x/namespace.h>
#include <x/gnutls/pubkeyobj.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {

		//! A public key

		//! This is a generic reference to a public key.
		//! \code
		//! pubkey pk;
		//!
		//! switch (pk->get_pk_algorithm()) {
		//! case GNUTLS_PK_RSA:
		//!     rsakeyfunc(gnutls::rsapubkey(pk));
		//!     break;
		//! case GNUTLS_PK_DSA:
		//!     dsakeyfunc(gnutls::dsapubkey(pk));
		//!     break;
		//! }
		//! \endcode

		typedef ptr<pubkeyobj> pubkey;

		//! A reference to a constant public key

		//! \see #pubkey
		//!

		typedef const_ptr<pubkeyobj> const_pubkey;
	}
}
#endif
