/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_serverauth_H
#define x_http_serverauth_H

#include <x/http/serverauthfwd.H>
#include <x/http/serverauthobj.H>
#include <x/gcrypt/md.H>
#include <x/ptr.H>
#include <x/ref.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

//! Base class for \ref serverauth.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::http::serverauth::base

class serverauthBase : public ptrref_base {

public:

	//! Compute the "A1" hash used by RFC 2617 digest authentication from a plaintext password.

	//! Returns the hash, as a hexadecimal string.

	static std::string compute_a1(//! Hash algorithm
				      gcry_md_algos algorithm,

				      //! Userid
				      const std::string &userid,

				      //! Password
				      const std::string &password,

				      //! Authentication realm
				      const std::string &realm);

};

#if 0
{
#endif
}

#endif
