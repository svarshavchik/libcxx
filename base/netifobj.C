/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/netif.H"
#include "x/strtok.H"
#include "x/sysexception.H"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <net/if_arp.h>

#ifndef SIOCGIFHWADDR
#include <net/if_dl.h>
#endif

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

netifObj::netifObj(const std::string &nameArg,
		   const std::vector<ifaddr> &ifaddrsArg,
		   const std::vector<unsigned char> &hwArg,
		   int indexArg,
		   short fflagsArg
		   ) noexcept
	: name(nameArg), ifaddrs(ifaddrsArg), hw(hwArg),
	  index(indexArg), fflags(fflagsArg)
{
}

netifObj::~netifObj()
{
}

void netifmakemap(std::map<std::string, std::vector<netif::base::ifaddr> > &ifmap,
		  struct ifaddrs *p) LIBCXX_INTERNAL;

void netifmakemap(std::map<std::string, std::vector<netif::base::ifaddr> > &ifmap,
		  struct ifaddrs *p)
{
	for (; p; p=p->ifa_next)
	{
		if (p->ifa_addr == NULL || p->ifa_netmask == NULL)
			continue;

		switch (p->ifa_addr->sa_family) {
		case AF_INET:
		case AF_INET6:
			break;
		default:
			continue;
		}

		switch (p->ifa_netmask->sa_family) {
		case AF_INET:
		case AF_INET6:
			break;
		default:
			continue;
		}

		ifmap[p->ifa_name]
			.push_back(netif::base::ifaddr(sockaddr::create(p->ifa_addr),
						       sockaddr::create(p->ifa_netmask)
						       ));
	}
}

void init_ifr(struct ifreq &ifr, const std::string &n) noexcept LIBCXX_INTERNAL;

void init_ifr(struct ifreq &ifr, const std::string &n) noexcept
{
	ifr=ifreq();
	strncat(ifr.ifr_name, n.c_str(), sizeof(ifr.ifr_name)-1);
}

#ifdef SIOCGIFHWADDR

std::vector< netif > netifBase::enumerate()
{
	std::vector< netif > interfaces;

	fd sock=fd::base::socket(AF_INET, SOCK_STREAM, 0);

	struct ifaddrs *iflist;

	if (getifaddrs(&iflist) < 0)
		throw SYSEXCEPTION("getifaddrs");

	std::map<std::string, std::vector<ifaddr> > ifmap;

	try {
		netifmakemap(ifmap, iflist);
	} catch (...) {
		freeifaddrs(iflist);
		throw;
	}
	freeifaddrs(iflist);

	interfaces.reserve(ifmap.size());

	for (std::map<std::string, std::vector<ifaddr> >::const_iterator
		     b(ifmap.begin()),
		     e(ifmap.end()); b != e; ++b)
	{
		struct ifreq i;

		init_ifr(i, b->first);

		short fflags=0;

		if (ioctl(sock->get_fd(), SIOCGIFFLAGS, &i) >= 0)
			fflags=i.ifr_flags;

		init_ifr(i, b->first);

		int index= -1;
		if (ioctl(sock->get_fd(), SIOCGIFINDEX, &i) >= 0)
			index=i.ifr_ifindex;

		init_ifr(i, b->first);

		std::vector<unsigned char> hw;

		int l=0;

		if (ioctl(sock->get_fd(), SIOCGIFHWADDR, &i) >= 0)
		{
			switch (i.ifr_hwaddr.sa_family) {
			case ARPHRD_ETHER:
				l=6; // ETH_ALEN;
				break;
			case ARPHRD_ARCNET:
				l=1; // ARCNET_ALEN:
				break;
			case ARPHRD_FCPP:
			case ARPHRD_FCAL:
			case ARPHRD_FCPL:
			case ARPHRD_FCFABRIC:
				l=6; // FC_ALEN;
				break;
			case ARPHRD_FDDI:
				l=6; // FDDI_ALEN;
				break;
			case ARPHRD_HIPPI:
				l=6; // HIPPI_ALEN;
				break;
			case ARPHRD_INFINIBAND:
				l=20; // INFINIBAND_ALEN;
				break;
			case ARPHRD_LOCALTLK:
				l=1; // LOCALTLK_ALEN;
				break;
			case ARPHRD_PRONET:
				l=6; // TR_ALEN;
				break;
			default:
				break;
			}
		}

		hw.resize(l);

		if (l)
			memcpy(&hw[0], i.ifr_hwaddr.sa_data, l);

		interfaces.push_back(netif::create(b->first,
						   b->second, hw,
						   index,
						   fflags));
	}

	return interfaces;
}

#else
std::vector< netif > netifBase::enumerate()
{
	std::vector< netif > interfaces;

	fd sock=fd::base::socket(AF_INET, SOCK_STREAM, 0);

	struct ifaddrs *iflist;

	if (getifaddrs(&iflist) < 0)
		throw SYSEXCEPTION("getifaddrs");

	std::map<std::string, std::vector<ifaddr> > ifmap;
	std::map<std::string, std::vector<unsigned char> > hwmap;

	try {
		netifmakemap(ifmap, iflist);

		for (struct ifaddrs *p=iflist; p; p=p->ifa_next)
		{
			struct sockaddr_dl *sdl =
				(struct sockaddr_dl *) p->ifa_addr;

			if (sdl == NULL || sdl->sdl_family != AF_LINK)
				continue;

			unsigned char *mac=(unsigned char *)LLADDR(sdl);

			hwmap[p->ifa_name]=std::vector<unsigned char>
				(mac, mac+sdl->sdl_alen);
		}
	} catch (...) {
		freeifaddrs(iflist);
		throw;
	}
	freeifaddrs(iflist);

	interfaces.reserve(ifmap.size());

	for (std::map<std::string, std::vector<ifaddr> >::const_iterator
		     b(ifmap.begin()),
		     e(ifmap.end()); b != e; ++b)
	{
		struct ifreq i;

		init_ifr(i, b->first);

		short fflags=0;

		if (ioctl(sock->get_fd(), SIOCGIFFLAGS, &i) >= 0)
			fflags=i.ifr_flags;

		init_ifr(i, b->first);

		int index= -1;
		if (ioctl(sock->get_fd(), SIOCGIFINDEX, &i) >= 0)
			index=i.ifr_index;

		interfaces.push_back(netif::create(b->first,
						   b->second,
						   hwmap[b->first],
						   index,
						   fflags));
	}

	return interfaces;
}

#endif
#if 0
{
#endif
}
