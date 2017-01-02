/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/netaddrobj.H"
#include "x/netaddrresult.H"
#include "x/fdtimeoutconfig.H"
#include "x/fd.H"
#include "x/fdtimeoutobj.H"
#include "x/sysexception.H"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <map>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

netaddrResultObj::netaddrResultObj() noexcept
{
}

netaddrResultObj::~netaddrResultObj()
{
}

fd netaddrResultObj::connect()
	const
{
	return connect(fdtimeoutconfig());
}

netaddrResultObj::addrstruct::addrstruct(const const_sockaddr &addrArg)
 : addr(addrArg)
{
}

netaddrResultObj::addrstruct::~addrstruct()
{
}

fd netaddrResultObj::connect(const fdtimeoutconfig &timeoutConfig)
	const
{
	bool hasException=false;
	sysexception lastException;

	for (auto as:addrlist)
	{
		fd new_socket(fd::base::socket(as.domain, as.type,
					       as.protocol));

		fdbase connect_socket(timeoutConfig(new_socket));

		try {
			connect_socket->pubconnect(as.addr);

			return new_socket;
		} catch (const sysexception &e)
		{
			lastException=e;
			hasException=true;
		}
	}

	if (!hasException)
	{
		errno=ETIMEDOUT;

		throw SYSEXCEPTION("connect");
	}

	throw lastException;
}

void netaddrResultObj::bind(std::list<fd> &fds,
			    bool reuseaddr) const
{
	fds.clear();

	// Bind IPv6 addresses first. If an IPv6 address is an IPv6-mapped
	// IPv4 address, a wildcard or a loopback address, make sure that the
	// ipv6only flag is off, and if there's an IPv4 equivalent in the list,
	// remove it.

	std::set<addrstruct> wildcard_portmap;

	for (auto as:addrlist)
	{
		if (as.domain != AF_INET6)
			continue;

		fd new_socket(fd::base::socket(as.domain, as.type, as.protocol));

		new_socket->ipv6only(false);
		new_socket->reuseaddr(reuseaddr);
		new_socket->bind(as.addr);
		fds.push_back(new_socket);

		sockaddr boundName=new_socket->getsockname();

		std::string name= boundName->address();

		struct sockaddr_in sin=sockaddr_in();

		sin.sin_family=AF_INET;

		sin.sin_port=ntohs(as.addr->port());

		if (name.substr(0, 7) == "::ffff:")
		{
			errno=EINVAL;

			if (inet_pton(AF_INET, name.substr(7).c_str(),
				      &sin.sin_addr) <= 0)
				throw SYSEXCEPTION(name.substr(7));
		}
		else if (name == "::1")
		{
			sin.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
		}
		else
		{
			if (name != "::")
				continue;
			sin.sin_addr.s_addr=htonl(INADDR_ANY);
		}

		addrstruct ipv4(sockaddr::create(reinterpret_cast
						       <struct ::sockaddr *>
						       (&sin)));

		ipv4.domain=AF_INET;
		ipv4.type=as.type;
		ipv4.protocol=as.protocol;

		if (!wildcard_portmap.insert(ipv4).second)
		{
			errno=EADDRINUSE;
			throw SYSEXCEPTION("bind("
					   + (std::string)*ipv4.addr
					   + ")");
		}
	}

	for (auto as:addrlist)
	{
		if (as.domain == AF_INET6)
			continue;

		if (wildcard_portmap.find(as) != wildcard_portmap.end())
			continue;

		fd new_socket(fd::base::socket(as.domain, as.type, as.protocol));

		new_socket->reuseaddr(reuseaddr);
		new_socket->bind(as.addr);
		fds.push_back(new_socket);
	}
}

netaddrResultObj &netaddrResultObj::add(const const_sockaddr &addr, int type)
{
	addrstruct a(addr);

	a.domain=addr->family();
	a.type=type;
	a.protocol=0;

	addrlist.push_back(a);
	return *this;
}

#if 0
{
#endif
}
