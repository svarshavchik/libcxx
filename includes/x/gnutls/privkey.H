/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_privkey_H
#define x_tls_privkey_H

#include <x/namespace.h>
#include <x/gnutls/privkeyobj.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE::gnutls {

	//! A private key

	//! This is a generic reference to a private key.
	//! \code
	//! privkey pk;
	//!
	//! switch (pk->get_pk_algorithm()) {
	//! case GNUTLS_PK_RSA:
	//!     rsakeyfunc(gnutls::rsaprivkey(pk),
	//!                gnutls::rsapubkey(pk));
	//!     break;
	//! case GNUTLS_PK_DSA:
	//!     dsakeyfunc(gnutls::dsaprivkey(pk),
	//!                gnutls::dsapubkey(pk));
	//!     break;
	//! }
	//! \endcode
	//!
	//! \see #rsaprivkey, #dsaprivkey

	typedef ptr<privkeyobj> privkey;

	//! A reference to a constant private key

	//! \see #pubkey
	//!

	typedef const_ptr<privkeyobj> const_privkey;
}
#endif
