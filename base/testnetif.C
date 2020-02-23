/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/netif.H"
#include "x/sysexception.H"
#include <arpa/inet.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>

static void testnetif()
{
	std::vector<LIBCXX_NAMESPACE::netif> interfaces;

	LIBCXX_NAMESPACE::netif::base::enumerate(interfaces);

	if (interfaces.empty())
		throw EXCEPTION("Could not enumerate network interfaces");

	for (std::vector<LIBCXX_NAMESPACE::netif>::iterator b(interfaces.begin()),
		     e(interfaces.end()); b != e; ++b)
	{
		std::cout << "interface: " << (*b)->getName() << std::endl;
		std::cout << "    index: " << (*b)->getIndex() << std::endl;
		std::cout << "    flags: " << std::setw(sizeof(short)*2)
			  << std::setfill('0') << std::hex
			  << (*b)->getFlags() << std::setw(0)
			  << std::dec << std::endl;

		std::cout << "addresses: ";

		const char *sep="";

		for (auto &netif : (*b)->getAddrs())
		{
			std::cout << sep
				  << (std::string)*netif.addr;
			if (netif.netmask->size())
				std::cout << "/" << (std::string)*netif.netmask;
			sep=", ";
		}
		std::cout << std::endl << std::endl;
	}



	for (std::vector<LIBCXX_NAMESPACE::netif>::iterator b(interfaces.begin()),
		     e(interfaces.end()); b != e; ++b)
	{
		for (std::vector<LIBCXX_NAMESPACE::netif::base::ifaddr>::const_iterator
			     ab((*b)->getAddrs().begin()),
			     ae((*b)->getAddrs().end()); ab != ae; ++ab)
		{
			for (int n=1024; n<3000; n++)
			{
				LIBCXX_NAMESPACE::fdptr listenSock;

				LIBCXX_NAMESPACE::sockaddrptr addr;

				try {
					listenSock=LIBCXX_NAMESPACE::fd::base::socket(ab->addr
								 ->family(),
								 SOCK_STREAM,
								 0);
					addr=LIBCXX_NAMESPACE::sockaddr::create(ab->addr
								 ->family(),
								 ab->addr
								 ->address(),
								 n);

					listenSock->bind(addr);
				} catch (...) {
					continue;
				}
				listenSock->listen();

				LIBCXX_NAMESPACE::fd connectSock(LIBCXX_NAMESPACE::fd::base::socket(ab->addr->
								family(),
								SOCK_STREAM,
								0));

				connectSock->nonblock(true);

				try {
					connectSock->connect(addr);
				} catch (const LIBCXX_NAMESPACE::sysexception &e)
				{
					if (e.getErrorCode() != EINPROGRESS)
						throw;
				}

				LIBCXX_NAMESPACE::fd connSock(listenSock->accept());

				LIBCXX_NAMESPACE::sockaddr sockname(connSock->getsockname());
				LIBCXX_NAMESPACE::sockaddr peername(connSock->getpeername());

				std::cout << "Socket: "
					  << (std::string)*sockname
					  << ", peer: "
					  << (std::string)*peername
					  << std::endl;

				std::cout << "Address: "
					  << sockname->address()
					  << " <=> "
					  << peername->address()
					  << std::endl;

				if (sockname->address() != peername->address())
					throw EXCEPTION("Addresses differ");
				return;
			}
		}
	}

	throw EXCEPTION("Unable to establish a test socket connection on any network interface");
}

int main(int argc, char **argv)
{
	alarm(10);

	try {
		testnetif();
	} catch (LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}
