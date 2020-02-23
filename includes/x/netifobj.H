/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_netifobj_H
#define x_netifobj_H

#include <x/fd.H>
#include <x/obj.H>
#include <x/sockaddr.H>
#include <net/if.h>

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Information about a network interface

class netifObj : virtual public obj {

	//! Network interface's name

	std::string name;

	//! Initialize an ifreq object, populated with 'name'

	void init(struct ifreq &ifr) const;

public:

	//! The object which holds the network interface's assigned addres
	struct ifaddr {

		//! The network link address
		const_sockaddr addr;

		//! The network link netmask
		const_sockaddr netmask;

		//! Constructor
		ifaddr(const const_sockaddr &addrArg,
		       const const_sockaddr &netmaskArg)
			: addr(addrArg), netmask(netmaskArg)
		{
		}

		~ifaddr() {}

	};

private:

	//! The list of network addresses

	std::vector<ifaddr> ifaddrs;

	//! MAC address
	std::vector<unsigned char> hw;

	//! Hardware index
	int index;

	//! Flags
	short fflags;

public:
	//! Internal constructor

	netifObj(//! Interface name
		 const std::string &nameArg,

		 //! Addresses
		 const std::vector<ifaddr> &ifaddrsArg,

		 //! MAC address
		 const std::vector<unsigned char> &hwArg,

		 //! Index
		 int indexArg,

		 //! Flags
		 short fflagsArg
		 ) noexcept;

	//! Destructor
	~netifObj();

	//! Return the interface's name

	const std::string &getName() const noexcept
	{
		return name;
	}

	//! Return the list of defined addresses on this network interface

	const std::vector<ifaddr> &getAddrs() const noexcept
	{
		return ifaddrs;
	}

	//! The SIOCGIFFLAGS ioctl

	short getFlags() const
	{
		return fflags;
	}

	//! The SIOCGIFINDEX ioctl

	int getIndex() const
	{
		return index;
	}

	//! The SIOCGIFHWADDR ioctl

	const std::vector<unsigned char> &getMacAddress()
		const
	{
		return hw;
	}
};

#if 0
{
#endif
}
#endif
