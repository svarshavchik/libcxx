/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_tls_datumwrapper_H
#define x_tls_datumwrapper_H

#include <x/namespace.h>
#include <x/vector.H>

#include <x/gnutls/init.H>
#include <gnutls/gnutls.h>

namespace LIBCXX_NAMESPACE::gnutls {

	//! The \c C++ -ified representation of a gnutls_datum_t

	//! datum_t is a reference to a vector of \c unsigned \c chars.
	//! The vector is reference-counted, and this reference
	//! can be freely passed around. The vector gets destroyed
	//! automatically, when the last reference goes out of scope.
	typedef vector<unsigned char> datum_t;

	//! A constant reference

	//! \see datum_t
	//!
	typedef const_vector<unsigned char> const_datum_t;

	//! A nullable pointer to a datum

	typedef vectorptr<unsigned char> datumptr_t;

	//! A nullable pointer to a constant datum

	//! \see datum_t
	//!
	typedef const_vectorptr<unsigned char> const_datumptr_t;

	class datumwrapper;
#if 0
}
#endif

//! A wrapper for \c gnutls_datum_t

//! \internal
//! This object takes ownership of \c gnutls_datum_t -s that are returned
//! by GnuTLS functions.
//! The destructor automatically frees memory. Convert to
//! \c vector<char> upon demand.

class datumwrapper {

public:
	//! The \c gnutls_datum_t value returned from some GnuTLS function.

	gnutls_datum_t datum;

	//! Default constructor
	datumwrapper(//! Return value from a GnuTLS function

		     const gnutls_datum_t &valueRef) noexcept;

	//! Going back the opposite way: from a reference to a datum

	datumwrapper(datum_t importRef);

	//! Deallocate the \c gnutls_datum_t via gnutls_free
	~datumwrapper();

	//! Convert to a vector, upon demand.
	operator datum_t() const;


	//! Convert GnuTLS-managed \c gnutls_datum_t

	//! When we access a \c gnutls_datum_t whose deallocation is
	//! GnuTLS library's responsibility, convert it directly to a
	//! datum_t without claiming ownership of it.

	static datum_t convert(const gnutls_datum_t &valueRef);

};

//! Convert a datum_t to a temporary \c gnutls_datum_t that will be passed to GnuTLS, then discarded

class tempdatum {

public:
	//! The \c gnutls_datum_t that will be passed to GnuTLS

	gnutls_datum_t datum;

	//! A datum_t that we own

	tempdatum(const const_datum_t &datum) noexcept;
	~tempdatum() {}
};

#if 0
#endif
}
#endif
