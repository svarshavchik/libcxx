/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_netaddrresultobj_H
#define x_netaddrresultobj_H

#include <x/obj.H>
#include <x/fd.H>
#include <x/fdfwd.H>
#include <x/netaddrfwd.H>
#include <x/namespace.h>
#include <x/sockaddr.H>
#include <x/exception.H>
#include <vector>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class fdtimeoutconfig;

//! Resolved socket addresses.

//! This object holds a list of socket addresses returned by the getaddrinfo(3)
//! library function. This object is not accessed directly, but rather using
//! the \link netaddrBase::result netaddr::base::result \endlink reference.

class netaddrResultObj : virtual public obj {

public:
	//! Default constructor.

	//! The constructor initializes an empty list of socket addresses.
	//!
	netaddrResultObj() noexcept;

	//! The destructor.

	//! The list of addresses is released and all memory gets freed.
	//!
	~netaddrResultObj();

	//! A single socket address.

	//! This small object holds the parameters that are used to create
	//! a socket, and the actual, raw network address for the bind(2) or
	//! the connect(2) system call.
	//!
	//! This type is usually references as \c netaddr::result::addrstruct.
	//!
	class addrstruct {
	public:
		//! Address family this socket belongs to.

		//! The address family may be \c AF_INET, \c AF_INET6, or
		//! other address family.
		//! This is the first parameter to the socket(2) library
		//! function.
		int domain;

		//! Socket type.

		//! The socket type is usually \c SOCK_STREAM or
		//! \c SOCK_DGRAM. This is the second parameter to the
		//! socket(2) library function.
		int type;

		//! Socket protocl.

		//! This is the address family-specific protocol. This is the
		//! third parameter to the socket(2) library function.

		int protocol;

		//! The socket address.

		//! This is the actual socket address, for the socket(2)
		//! or the bind(2) system call.
		const_sockaddr addr;

		//! Constructor
		addrstruct(const const_sockaddr &addrArg);

		//! Destructor
		~addrstruct();

		//! Comparison operator

		bool operator==(const addrstruct &o) const
		{
			return domain == o.domain &&
				type == o.type &&
				protocol == o.protocol &&
				*addr == *o.addr;
		}

		//! Comparison operator

		bool operator<(const addrstruct &o) const
		{
			if (domain < o.domain)
				return true;

			if (domain > o.domain)
				return false;

			if (type < o.type)
				return true;

			if (type > o.type)
				return false;

			if (protocol < o.protocol)
				return true;

			if (protocol > o.protocol)
				return false;

			return *addr < *o.addr;
		}

	};

	//! The canonical server's name.

	//! This canonical name gets returned by getaddrinfo(3), where
	//! available.
	std::string canonname;

	//! A container for the list of socket addresses

	typedef std::list<addrstruct> addrlist_t;

	//! The list of socket addresses.

	//! Depending on the input parameters, getaddrinfo(3) may return
	//! more than one socket addess. All addresses are placed into
	//! this list.
	addrlist_t addrlist;

	//! Retrieve the beginning iterator.

	//! \return the address list's beginning iterator.
	//!
	addrlist_t::const_iterator begin() const noexcept
	{
		return addrlist.begin();
	}

	//! Retrieve the ending iterator.

	//! \return the address list's ending iterator.
	//!
	addrlist_t::const_iterator end() const noexcept
	{
		return addrlist.end();
	}

	//! Connect to the resolved network address.

	//! This function connects to the resolved network address. If
	//! more than one network address was resolved, a connection attempt
	//! is made until it succeeds.
	//!
	//! \return the connected socket.
	//! An %exception gets thrown if a connected socket cannot be
	//! established to any resolved address.

	fd connect() const;

	//! Connect to the resolved network address.

	//! \overload
	//!
	fd connect(//! Timeout configuration object for the connection attempt
		   const fdtimeoutconfig &timeoutConfig) const;

public:
	//! Bind a server socket.

	//! This function creates one or more sockets and binds them to the
	//! resolved network addresses.
	//!
	//! If more than one network address is resolved, a separate socket
	//! gets created for each resolved network address.
	//!
	//! \note
	//! This function handles the sticky situation of mixed IPv4/IPv6
	//! binds. If an IPv4 or IPv6 socket is already bound
	//! on a given port, an attempt to bind another socket to the
	//! same port, but on the other address family, fails.
	//! Normally, an error gets thrown if a socket bind fails, but
	//! this function catches this situation and handles it.
	//! Since IPv6 sockets accepts connections to IPv4-compatible
	//! IP addresses, this function binds all IPv6 addresses first, followed
	//! by IPv4 addresses. An error binding an IPv4 address is quietly
	//! ignored if the same port is already succesfully bound on the IPv6
	//! side.
	//!
	//! \note
	//! Failure to bound a port results in a thrown %exception (with the
	//! exception described above). This function succeeds only if all
	//! addresses were bound.
	void bind(//! Bound sockets.

		  //! The list of sockets, one for each bound port, is placed
		  //! into this list. This list gets cleared, before populating
		  //! it with sockets.
		  //!
		  //! \note
		  //! Due to the IPv4/IPv6 issue described above, this list
		  //! may not have the exact same number of sockets as there
		  //! are socket addresses in this address list (but that's
		  //! usually the case).
		  std::list<fd> &fds,

		  //! \c SO_REUSEADDR flag.

		  //! is set for each created socket.
		  //! If this parameter is true, the \c SO_REUSEADDR flag
		  bool reuseaddr) const;

	//! Combine two address lists together

	netaddrResultObj &operator+=(const netaddrResultObj &o)

	{
		addrlist.insert(addrlist.end(), o.begin(), o.end());
		return *this;
	}

	//! Manually add an address.

	netaddrResultObj &add(const const_sockaddr &addr,
			      int type=SOCK_STREAM);

	//! Combine two address lists together

	netaddr::base::result operator+(const netaddrResultObj &o)
;
};

#if 0
{
#endif
}
#endif
