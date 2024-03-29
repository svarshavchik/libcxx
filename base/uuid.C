/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/uuid.H"
#include "x/netif.H"
#include "x/exception.H"
#include "x/fd.H"
#include "x/locale.H"
#include "x/strtok.H"
#include "x/netif.H"

#include <sys/ioctl.h>
#include <net/if.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::uuid);

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

template class base64alphabet<>;
template class base64<>;
template class base64<0>;

template class base64alphabet<',', '_'>;
template class base64<0, base64alphabet<',', '_'>>;

uuid::macObj::macObj()
{
	std::vector<netif> interfaces=netif::base::enumerate();

	for (std::vector<netif>::iterator b(interfaces.begin()),
		     e(interfaces.end()); b != e; ++b)
	{
		std::vector<unsigned char> hwaddr;

		try {
			if ((*b)->getFlags() & IFF_LOOPBACK)
				continue;

			hwaddr=(*b)->getMacAddress();
		} catch (...) {
			continue;
		}

		if (hwaddr.size() != sizeof(parts.uuid_hwaddr))
			continue;

		mac=hwaddr;

		LOG_DEBUG("Retrieved MAC address from " << (*b)->getName());
		break;
	}
}

singleton<uuid::macObj> uuid::mac;

uuid::macObj::~macObj()
{
}

bool __thread uuid::uuid_init=false;

__thread struct uuid::uuid_parts uuid::parts;

uuid::uuid()
{
	if (!uuid_init)
	{
		init();
		uuid_init=true;
	}

	if ((++parts.uuid_random & 65535) == 0)
	{
		struct timeval tv;

		gettimeofday(&tv, NULL);
		parts.uuid_tv_sec=tv.tv_sec;
		parts.uuid_tv_usec=tv.tv_usec;
	}

	memcpy((char *)val, (char *)&parts, sizeof(val));
}

uuid::~uuid()
{
}

void uuid::init()
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	parts.uuid_tv_sec=tv.tv_sec;
	parts.uuid_tv_usec=tv.tv_usec;
	parts.uuid_tid=gettid();

	refmac rmac(mac.get());

	if (rmac->mac.size() == sizeof(parts.uuid_hwaddr))
		std::copy(rmac->mac.begin(), rmac->mac.end(),
			  parts.uuid_hwaddr);

	fd sock=fd::base::open("/dev/urandom", O_RDONLY);

	sock->read( (char *)&parts.uuid_random,
		    sizeof(parts.uuid_random));

	LOG_INFO("Initialized unique id generator for thread " <<
		 parts.uuid_tid);
}

void uuid::asString(charbuf cb) const noexcept
{
	*base64_t::encode( (char *)&val,
			   (char *)&val[sizeof(val)/sizeof(val[0])],
			   cb, 0, false)=0;
}

uuid &uuid::operator=(const std::string_view &str)
{
	decode(str);
	return *this;
}

uuid::uuid(const std::string_view &str)
{
	decode(str);
}

void uuid::decode(const std::string_view &str)
{
	size_t l=str.size();
	size_t s=base64_t::decoded_size(l);

	if (s < sizeof(val)+sizeof(val[0])*2)
	{
		char buf[sizeof(val)+sizeof(val[0])*2];

		auto [ptr, flag]=base64_t::decode(str.begin(), str.end(),
						  &buf[0]);

		if (flag && ptr - buf == sizeof(val))
		{
			std::copy(buf, buf+sizeof(val), (char *)&val);
			return;
		}
	}

	throw EXCEPTION("Invalid UUID");
}

#if 0
{
#endif
}
