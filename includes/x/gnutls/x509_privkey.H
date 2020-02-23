/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_x509_privkey_H
#define x_tls_x509_privkey_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/gnutls/x509_privkeyobj.H>
#include <x/gnutls/x509_privkeyfwd.H>

namespace LIBCXX_NAMESPACE {

	namespace gnutls {

		//! X.509 certificates

		namespace x509 {
#if 0
		};
	};
};
#endif

//! Refer to this class is \c gnutls::x509::privkey::base or  \c gnutls::x509::privkeyptr::base

class privkeyBase : public ptrref_base {

public:

	//! Retrieve the list of all known public key algorithms

	//! This function retrieves the list of all public key
	//! algorithms implemented by GnuTLS.
	//!
	//! \code
	//! std::list<gnutls_pk_algorithm_t> pk_list;
	//!
	//! INSERT_LIBX_NAMESPACE::gnutls::x509::privkey::get_algorithm_list(pk_list);
	//! \endcode
	//!
	//! See also:
	//! \ref get_algorithm_name "INSERT_LIBX_NAMESPACE::gnutls::x509::privkey::get_algorithm_name"(),
	//! \ref get_algorithm_id "INSERT_LIBX_NAMESPACE::gnutls::x509::privkey::get_algorithm_id"().

	static
	void get_algorithm_list(//! This list gets filled with the known public key algorithms
				std::list<gnutls_pk_algorithm_t> &listRet)
		noexcept;

	//! Return the name of a public key algorithm

	//! \code
	//! std::string n=INSERT_LIBX_NAMESPACE::gnutls::session::get_algorithm_name(GNUTLS_PK_RSA);
	//! \endcode
	//!
	//! \return the algorithm's name. An %exception gets thrown if
	//! the algorithm is not known.
	static
	std::string get_algorithm_name(//! Public key algorithm
				       gnutls_pk_algorithm_t a);

	//! Return the public key algorithm id given its name

	//! \code
	//! gnutls_pk_algorithm_t algo=
	//!     INSERT_LIBX_NAMESPACE::gnutls::session::get_algorithm_id("RSA");
	//! \endcode

	//! \return the algorithm ID. An %exception gets thrown if the
	//! name is not recognized.
	static
	gnutls_pk_algorithm_t get_algorithm_id(//! Algorithm's name
					       const std::string &name);
};

#if 0
{
	{
		{
#endif
		}
	}
}

#endif
