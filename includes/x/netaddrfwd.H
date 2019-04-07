/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_netaddrfwd_H
#define x_netaddrfwd_H

#include <x/ptrfwd.H>
#include <x/fdfwd.H>
#include <x/namespace.h>

#include <sys/types.h>
#include <sys/socket.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class netaddrObj;
class netaddrBase;
class netaddrResultObj;
class netaddrResultBase;

class fdtimeoutconfig;

//! An unresolved network address.
//!
//! Connecting to a network socket, or accepting connections to a socket,
//! is a process that involves several steps.
//!
//! - define the network address
//! - resolve the network address
//! - open a socket and connect it (for clients), or begin accepting connections on the socket (server).
//!
//! \par Defining the network address
//!
//! This is the first step. The network address is defined as a hostname and/or
//! a port number. This is done using the ::netaddr object.
//!
//! ::netaddr is a reference to an automatically-allocated netaddrObj, and,
//! as with \ref ptr "reference counted objects", the reference
//! may freely be passed around:
//!
//! \code
//! netaddr httpaddr(netaddr::create("www.example.com", "http"));
//! \endcode
//!
//! \c httpaddr does not express \c www.example.com's IP address and
//! port number, just the hostname and port, as separate entities.
//! \c www.example.com may very well have multiple IP addresses. Resolving
//! the IP addresses happens later, this step only sets the hostname and the
//! port number.
//!
//! Some services have both a \c TCP and a \c UDP implementation, and by default
//! the host/port number may end up getting resolved to multiple sockets,
//! one \c TCP socket and one \c UDP socket. In most cases, either one or the
//! other is preferred, so the socket type should be forced:
//! \code
//! netaddr dnslookup(netaddr::create("dns.example.com", "domain", SOCK_STREAM));
//! \endcode
//!
//! \note The service can also be specified as a port number, not a service
//! name.
//!
//! There are separate methods that set various parameters of the
//! unresolved address. The above example can also be implemented like this:
//! \code
//! netaddr dnslookup;
//!
//! dnslookup->node("www.example.com");
//! dnslookup->service("domain");
//! dnslookup->type(SOCK_STREAM);
//! \endcode
//!
//! Each parameter function returns a reference to the same object, so the
//! above can be abbreviated like this:
//! \code
//! netaddr dnslookup;
//!
//! dnslookup->node("www.example.com")->service("domain")->type(SOCK_STREAM);
//! \endcode
//!
//! The available parameter functions are:
//!
//! - \link netaddrObj::node() netaddr->node(std::string) \endlink - set the hostname.
//! - \link netaddrObj::service() netaddr->service(std::string) \endlink - set the service name (which may be a numerical port number).
//! - \link netaddrObj::domain() netaddr->domain(int) \endlink - set the preferred address family: \c AF_INET, or \c AF_INET6.
//! - \link netaddrObj::type() netaddr->type(int) \endlink - set the preferred socket type: \c SOCK_STREAM, or \c SOCK_DGRAM.
//! - \link netaddrObj::protocol() netaddr->protocol(int) \endlink - set the socket type-specific protocol (normally not used).
//! - \link netaddrObj::flags() netaddr->flags(int) \endlink - set the address resolution flags.
//!
//! This object sets parameters for the getaddrinfo(3) function.
//! getaddrinfo(3) returns both IPv6 and IPv4 addresses, if available.
//! Use \link netaddrObj::domain() netaddr->domain(int) \endlink to limit
//! the resulting socket to an IPv4 or an IPv6 socket, only.
//!
//! \link netaddrObj::flags() netaddr->flags(int) \endlink sets the resolution
//! flags for getaddrinfo(3).
//!
//! The \link netaddrObj::node() netaddr->node(std::string) \endlink and
//! \link netaddrObj::service() netaddr->service(std::string) \endlink methods
//! (either explicitly or implicitly, via the constructor) do not always need
//! to be set.
//! Client sockets always require the service (port number). If the node name
//! is not set (or is given as an empty string in the constructor), the
//! client address gets resolved to localhost.
//! Server sockets also require the service name (port number). If the node
//! name is not set (or is given as an empty string in the constructor), the
//! socket binds to the wildcard address, else it gets bound to the specific
//! IP address(es) only.
//!
//! \par Construct an address using a unified specification
//!
//! \code
//!    std::string address;
//!    std::string default_service;
//!
//!    netaddr addrspec(netaddr::create(SOCK_STREAM, address, default_service));
//! \endcode
//!
//! This special \link netaddrObj::netaddrObj(int,const std::string &,const std::string&) create()\endlink
//! method allows creation of either network sockets or filesystem sockets,
//! using a unified address specification format.
//!
//! \par Resolving the address
//!
//! Once the address parameters are set, it's time to actually resolve to
//! one or more address structures. This is done by converting
//! a \c netaddr object into
//! a \link netaddrBase::result netaddr::base::result\endlink by
//! dereferencing it:
//!
//! \code
//! netaddr::result addrs(*httpaddr);
//!
//! for (netaddr::result::const_iterator b=addrs->begin(), e=addrs.end();
//!       b != e; ++b)
//! {
//!    netaddr::result::addrstruct resolved_address= *b;
//!
//!    // ...
//! }
//! \endcode
//!
//! Once a \link netaddrBase::result netaddr::base::result\endlink reference
//! exists, the
//! \link netaddrResultObj::connect() netaddr::result->connect() \endlink
//! or
//! \link netaddrResultObj::bind() netaddr::result->bind() \endlink
//! methods open the sockets.
//! See \link netaddrBase::result netaddr::base::result\endlink
//! for more information.
//!
//! \par Additional convenience functions
//!
//! \code
//! std::list<fd> fdlist;
//!
//! netaddr::create("", 4000, SOCK_STREAM)->bind(fdlist, true);
//! \endcode
//!
//! ::netaddr also defines two convenience functions:
//! \link netaddrObj::connect() netaddr->connect() \endlink and
//! \link netaddrObj::bind() netaddr->bind() \endlink.
//! These convenience functions resolve the address by instantiating a
//! \c netaddr::result object, then invoking the object's
//! \link netaddrResultObj::connect() connect() \endlink and
//! \link netaddrResultObj::bind() bind() \endlink
//! methods. This allows for a very compact syntax:
//!
//! \code
//! fd httpconn=netaddr::create("www.example.com","http",SOCK_STREAM)->connect();
//! \endcode
//!
//! \par Convenience functions for listening on multiple ports
//!
//! \code
//! std::list<fd> fdlist;
//!
//! netaddr::create("", "4000,4001", SOCK_STREAM)->bind(fdlist, true);
//! \endcode
//!
//! An empty string passed as a first parameter to create() creates an address
//! for a passive, listening socket. Multiple port numbers may be specified
//! by a second parameter that's a string
//! that contains a list of port numbers or names separated by commas or spaces.
//! This creates socket address structures for each
//! specified port. When
//! used with bind(), a listening socket gets created for each specified port.
//! When used with connect(), a connection is established to the first port
//! in the list that's accepting connections.
//!
//! \code
//! std::list<fd> fdlist;
//!
//! std::list<int> ports;
//!
//! netaddr::create("", ports, SOCK_STREAM)->bind(fdlist, true);
//! \endcode
//!
//! The second parameter may also be an explicit list of port numbers.

typedef ref<netaddrObj, netaddrBase> netaddr;

//! A nullable pointer to a netaddrObj

//! \see netaddr

typedef ptr<netaddrObj, netaddrBase> netaddrptr;

//! Refer to this class as \c netaddr::base or \c netaddrptr::base

//! \internal
//! This class provides definitions for member types of
//! \link ::netaddr netaddr\endlink, a reference to an object that holds
//! unresolved network addresses.
//!

class netaddrBase : public ptrref_base {

public:

	//! \c netaddr::result - a resolved network address.

	//! This type should be referenced as \c netaddr::result.

	//! \c netaddr::result is a reference to a resolved network address.
	//! This is a reference to a reference-counted
	//! netaddrResultObj class.
	//!
	//! This object holds one or more socket addresses returned by
	//! getaddrinfo(3). Note that an address lookup may return multiple
	//! socket addresses. They are all stored here, until they're put to
	//! some use.
	//!
	//! \par Resolving addresses
	//!
	//! \code
	//! netaddr sockaddr_name=netaddr::create("www.example.com", "http");
	//!
	//! netaddr::result sockaddr=sockaddr_name;
	//! \endcode
	//!
	//! Casting a #netaddr into a \c netaddr::result resolves the
	//! address and places it into the \c netaddr::result object.
	//! A resolution failure results in a thrown an %exception.
	//!
	//! \par Creating a client socket and connecting to a server
	//!
	//! \code
	//! netaddr sockaddr_name=netaddr::create("www.example.com", "http");
	//! netaddr::result sockaddr=sockaddr_name;
	//!
	//! fd sockaddr->connect();
	//! \endcode
	//!
	//! The
	//! \link netaddrResultObj::connect netaddr::result->connect() \endlink
	//! function creates a client socket and connects it to the server
	//! at the given address. If the address was resolved to multiple IP
	//! addresses, a connection attempt is made to each one, until it
	//! gets accepted. An %exception gets thrown if a connection attempt
	//! to every IP addresses failed.
	//!
	//! \par Creating a server socket.
	//!
	//! \code
	//! netaddr::result servaddr=netaddr::create("", "http");
	//!
	//! std::list<fd> sockets;
	//!
	//! servaddr->bind(sockets, true);
	//! \endcode
	//!
	//! The \link netaddrResultObj::bind() netaddr::result->bind() \endlink
	//! method creates a socket and binds it to the given port.
	//! If the specified address was resolved to multiple socket
	//! addresses, a separate socket gets bound for each socket address.
	//! Each created socket file descriptor is placed in the given list.
	//! \note
	//! The only thing that
	//! \link netaddrResultObj::bind() netaddr::result->bind() \endlink
	//! does is create a socket and bind it to a port. Server applications
	//! will need to iterate over each file descriptor and invoke its
	//! \link fdObj::listen() fd->listen() \endlink and
	//! \link fdObj::accept() fd->accept() \endlink functions to listen for
	//! and accept socket connections.
	//!
	//! \par Low level access to resolved network address objects.
	//!
	//! \code
	//! netaddr::result sockaddr=netaddr::create("www.example.com", "http");
	//!
	//! netaddr::result::const_iterator
	//!        b=sockaddr->addrlist.begin(),
	//!        e=sockaddr->addrlist.end();
	//! \endcode
	//!
	//! If neither \c connect() nor \c bind() fit the bill, the resolved
	//! addresses may be enumerated by iterating over the resolved addresses
	//! as shown above. Dereferenced iterators will access the
	//! netaddrResultObj::addrstruct object.

	typedef ref<netaddrResultObj, netaddrResultBase> result;

	//! A nullable pointer to a result object.

	//! \see result

	typedef ptr<netaddrResultObj, netaddrResultBase> resultptr;
};

#if 0
{
#endif
}
#endif
