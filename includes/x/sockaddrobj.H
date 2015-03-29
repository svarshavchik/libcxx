/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sockaddrobj_H
#define x_sockaddrobj_H

#include <x/exceptionfwd.H>
#include <x/obj.H>
#include <x/namespace.h>
#include <sys/socket.h>
#include <vector>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A wrapper for a network address structure.

//! This is a wrapper for a \c char vector that presumably holds a
//! network address, of some kind.

class sockaddrObj : public std::vector<char>, virtual public obj {

public:
	//! Throw an %exception for an unrecognized address

	//! This method throws a generic %exception when the network address
	//! is not recognized.

	void badfamily() const  __attribute__((noreturn))
		LIBCXX_INTERNAL;

	//! Constructor
	sockaddrObj() noexcept;

	//! Destructor
	~sockaddrObj() noexcept;

	//! Initialize this network address structure.

	//! This is generally the opposite of operator std::string(). Create
	//! a network address from a text-based representation.
	//! Any previous contents of this address structure are destroyed.
	sockaddrObj(//! An address family.

		    //! This parameter can be \c AF_UNIX,
		    //! \c AF_INET, or
		    //! \c AF_INET6. All other address families
		    //! throw an
		    //! %exception.
		    int af,

		    //! The address parameter.

		    //! This is the filename, for \c AF_UNIX,
		    //! otherwise an IP
		    //! address, as a text: "127.0.0.1" or "::1".
		    //!
		    //! An empty string for \c AF_INET and
		    //! \c AF_INET6 specifies a wildcard address.

		    const std::string &p,

		    //! Port number.

		    //! This parameter gets ignored for
		    //! \c AF_UNIX addresses.
		    //!
		    int n);

	//! Initialize this network address structure.

	//! Initialize from an opaque sockaddr structure
	//!
	sockaddrObj(const struct ::sockaddr *);

	//! Initialize this network address structure.

	//! Initialize from an opaque sockaddr_storage structure
	//!

	sockaddrObj(const struct ::sockaddr_storage *ptr);

private:
	//! Common code for sockaddr and sockaddr_storage constructors

	void init_addr(const struct ::sockaddr *) LIBCXX_INTERNAL;

public:
	//! Return the address family of this network address

	//! This method retrieves this address's network family.

	//! \return \c AF_INET, \c AF_INET6, or \c AF_UNIX. Other address
	//! families may also be returned, although they are not directly
	//! supported.
	int family() const;

	//! Ordering operator.

	//! \internal
	//! This operator is implemented in order to allow sorting of network
	//! addresses into some deterministic order. This operator orders
	//! network addresses by their address family, and all addresses within
	//! a family by doing a bytewise comparison of the network address
	//! structure.
	//! \note
	//! All addresses in the given address family are presumed to have
	//! the same size (else an %exception gets thrown).
	bool operator<(//! Some other network address
		       const sockaddrObj &o) const;

	//! Ordering operator.

	//! \internal
	//! This operator checks if two addresses are the same. This is
	//! done by comparing the results of operator std::string(), so any
	//! padding in a raw address structure gets ignored.
	bool operator==(//! Some other network address
			const sockaddrObj &o) const;

	//! Check if an IPv4 and IPv6 addresses are the same.

	//! \internal
	//! This method returns true if, between this address structure and
	//! some other address structure, one is an IPv4 address and the
	//! other is an IPv6 address that refers to the same IPv4 address.

	bool is46(//! Some other network address
		  const sockaddrObj &o) const;

	//! Return a text-based representation of this network address structure.

	//! \return For \c AF_UNIX addresses, returns address().
	//! \c AF_INET and \c AF_INET6 addresses, if port() is 0,
	//! returns \c "address", else returns \c "address/port", where
	//! \c "address" and \c "port" are the return values from address() and
	//! port().
	//! An %exception gets thrown for all other address families.
	//! The reason that the separator is a slash is
	//! because IPv6 addresses contain colons.
	operator std::string() const;

	//! Return the host address portion

	//! \return For \c AF_UNIX addresses, returns the filename path. For
	//! \c AF_INET and \c AF_INET6 addresses, returns \c "address".
	//! \c "address" is an IPv4 or an IPv6 address.

	std::string address() const;

	//! For anonymous local sockets, address() returns this

	static const char anonlocalsocket[];

	//! Return the port number

	//! \return For \c AF_UNIX addresses, returns 0. For
	//! \c AF_INET and \c AF_INET6 addresses, returns the numeric port
	//! number.
	//! An %exception gets thrown for all other address families.
	int port() const;

	//! Set the port number

	//! Changes the port number in the socket structure.

	//! Only works for
	//! \c AF_INET and \c AF_INET6 addresses.
	//! An %exception gets thrown for all other address families.

	void port(//! New port number
		  int portnum);
};

#if 0
{
#endif
}
#endif
