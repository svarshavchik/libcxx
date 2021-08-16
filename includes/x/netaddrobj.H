/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_netaddrobj_H
#define x_netaddrobj_H

#include <x/obj.H>
#include <x/fdfwd.H>
#include <x/netaddrfwd.H>
#include <x/netaddrresult.H>
#include <x/namespace.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <list>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! An unresolved network address.

//! This object holds an unresolved network address. Generally, this object
//! holds the input parameters to the getaddrinfo(3) library function:
//! a network hostname, and/or port number, and various address flags.
//! The constructor populates these parameters with sensible defaults, and
//! various methods set the individual parameters.
//!
//! Once all the parameters are set, the
//! <a href="#operatorresult"> operator netaddr::result()</a> invokes
//! getaddrinfo(3) and returns the resolved network address.
//!
//! Finally, two convenience functions, connect() and bind(), invoke
//! operator netaddr::result(), then invoke the corresponding methods on the
//! netaddrBase::resultObj
//! object, essentially going from an unresolved network
//! address to one or more connected sockets, in one shot.
//!
//! \note
//! The methods that set the individual parameters return a reference to the
//! the same object. This allows chained invocations of methods that set
//! multiple parameters. Example:
//! \code
//! fd sock=netaddr::create("www.example.com","http")->domain(AF_INET)->connect();
//! \endcode

class netaddrObj : virtual public obj {

	//! Network hostname

	//! This is an input parameter to getaddrinfo(3).
	//! See the getaddrinfo(3) man page for more information.
	std::string nodeval;

	//! Port number, or service name

	//! This is an input parameter to getaddrinfo(3).
	//! See the getaddrinfo(3) man page for more information.
	std::string serviceval;

	//! Address resolution flags

	//! This is an input parameter to getaddrinfo(3).
	//! See the getaddrinfo(3) man page for more information.
	//!
	//! The default address resolution flags are:
	//! \c AI_ADDRCONFIG|AI_IDN|AI_CANONNAME|AI_CANONIDN

	int flagsval;

	//! Preferred address family

	//! This is an input parameter to getaddrinfo(3).
	//! See the getaddrinfo(3) man page for more information.
	int domain_hint;

	//! Preferred socket type

	//! This is an input parameter to getaddrinfo(3).
	//! See the getaddrinfo(3) man page for more information.
	int type_hint;

	//! Preferred protocol

	//! This is an input parameter to getaddrinfo(3).
	//! See the getaddrinfo(3) man page for more information.
	int protocol_hint;

public:

	//! Default constructor

	netaddrObj() noexcept;

	//! Construct an unresolved address

	//! This is the same as using the default constructor, then invoking
	//! node(), service(), and type() methods to set the corresponding
	//! parameters.
	netaddrObj(//! Network hostname.
		   const std::string &node,

		   //! Port number or service name.
		   const std::string &service,

		   //! Preferred socket type.
		   int type=SOCK_STREAM) noexcept;

	//! Construct an unresolved address

	//! \overload
	//!
	netaddrObj(//! Network hostname
		   const std::string &node,

		   //! Port number
		   int service,

		   //! Preferred socket type.
		   int type=SOCK_STREAM) noexcept;

	//! Construct an unresolved address

	//! \overload
	//!
	netaddrObj(//! Network hostname
		   const std::string &node,

		   //! Port numbers to listen on

		   const std::list<int> &ports,

		   //! Preferred socket type.
		   int type=SOCK_STREAM) noexcept;

	//! Construct an unresolved address

	netaddrObj(//! SOCK_STREAM or SOCK_DGRAM.
		   int type,

		   //! Address specification.

		   //! Possible formats:
		   //!
		   //! - "file:path" - an \c AF_UNIX address.
		   //!
		   //! - "inet:host/port" - an Internet address. "/port"
		   //! is optional, defaults to default_service
		   //!
		   //! If none of the above, if the first character is "/",
		   //! it's an \c AF_UNIX address, otherwise it's an
		   //! Internet address

		   const std::string &address,

		   //! The default port, unless overriden in address

		   const std::string &default_service="");

	//! Destructor.
	~netaddrObj();

	//! Set the network hostname

	//! Either node() or service() (or possibly both) must be invoked,
	//! otherwise address resolution will fail.
	netaddr node(//! The network hostname

		     const std::string &n) noexcept
	{
		nodeval=n;
		return netaddr{this};
	}

	//! Get the network hostname

	std::string node() noexcept { return nodeval; }

	//! Set the network port

	//! Either node() or service() (or possibly both) must be invoked,
	//! otherwise address resolution will fail.
	netaddr service(//! Network port, as string, or service name.
			const std::string &s) noexcept
	{
		serviceval=s;
		return netaddr{this};
	}

	//! Set the network port

	//! \overload
	//!

	netaddr service(//! Network port, as an integer
			int port) noexcept;

	//! Get the network port

	std::string service() noexcept { return serviceval; }

	//! Set address resolution flags

	//! This methods set non-default address resolution flags.
	//! \see flagsval

	netaddr flags(//! getaddrinfo(3) flags

		      //! This is a bitmask, that may contain the following
		      //! flags:
		      //! - \c AI_NUMERICHOST
		      //! - \c AI_PASSIVE
		      //! - \c AI_ADDRCONFIG
		      //! ... and others.
		      int v) noexcept { flagsval=v; return netaddr{this}; }

	//! Get address resolution flags

	int flags() noexcept { return flagsval; }

	//! Set the default address family domain

	//! \see domain_hint
	//!
	netaddr domain(//! Preferred address family

		       //! Possible values are \c AF_UNIX, \c AF_INET, or
		       //! \c AF_INET6. Other values are also allowed, but
		       //! are not directly supported by other functions.
		       int v) noexcept { domain_hint=v; return netaddr{this}; }

	//! Get the default address family domain

	int domain() noexcept { return domain_hint; }

	//! Set the default socket type

	//! \see type_hint
	//!
	netaddr type(//! The new socket type, either \c SOCK_STREAM or \c SOCK_DGRAM.
		     int v) noexcept { type_hint=v; return netaddr{this}; }

	//! Get the default socket type

	int type() noexcept { return type_hint; }

	//! Set the default socket protocol

	//! \see protocol_hint
	netaddr protocol(//! A socket type-specific protocol number.
			 int v) noexcept
	{
		protocol_hint=v;
		return netaddr{this};
	}

	//! Get the default socket protocol

	int protocol() noexcept { return protocol_hint; }

	//! Resolve network address

	//! <a name="operatorresult"></a>This operator function resolves this address and returns a
	//! \ref netaddrBase::result "netaddr::base::result" object.
	//! An %exception gets thrown upon an error.
	//! \return \ref netaddrBase::result "netaddr::base::result",
	//! a reference to a netaddr::resultObj.
	//! \see get_results().
	operator netaddr::base::result() const;

	//! Resolve network address, then establish a socket connection

	//! This is a convenience function that calls operator netaddr::result()
	//! then invokes its
	//! \ref netaddrResultObj::connect() "connect"()
	//! method to connect to the resulting network address.
	//! \return a connected network socket. An %exception gets thrown if
	//! the address resolution fails, or the connection attempt fails.

	fd connect() const;

	//! Resolve network address, then establish a socket connection

	//! \overload
	//!
	fd connect(//! Timeout configuration object for the connection attempt
		   const fdtimeoutconfig &timeoutConfig);

	//! Resolve network address, then bind a socket to each resolved address

	//! This is a convenience function that calls operator netaddr::result()
	//! then invokes the
	//! netaddrBase::resultObj::bind()
	//! method to bind sockets to the resolved addresses.
	//! An %exception gets thrown if
	//! the address resolution fails, or a socket fails to bind, for some
	//! reason.
	//! \note
	//! This function automatically sets the \c AI_PASSIVE flag, before
	//! resolving the address. There's
	//! no need to set this flag explicitly.
	void bind(
		  //! Bound sockets.

		  //! Sockets created and bound by this method are placed
		  //! into this list.
		  //! \note This method clears any existing contents of this
		  //! list.
		  std::list<fd> &fds,

		  //! Whether to set the \c SO_REUSEADDR flag.

		  //! This parameter specifies whether or not to set the
		  //! \c SO_REUSEADDR flag for each bound socket.
		  bool reuseaddr) const;

private:
	//! Resolve network address

	//! \internal
	//! This function invokes getaddrinfo(3) and puts its results into
	//! a newly-constructed object referenced by a netaddr::result object.
	//! \note
	//! If the \c AI_PASSIVE flag is set, the \c AI_CANONNAME and
	//! \c AI_CANONIDN flags are cleared. glibc apparently rejects this
	//! flag combination, and it's clear that the latter two are unwanted
	//! if \c AI_PASSIVE is set, so they get cleared.

	netaddr::base::result
	get_results(//! Additional flags

		    //! This parameter sets any flags in
		    //! addition to what was specified by the
		    //! flags() method.
		    int extra_flags) const
		LIBCXX_INTERNAL;
};

#if 0
{
#endif
}
#endif
