/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "sockaddrobj.H"
#include "sysexception.H"
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <algorithm>
#include <cstring>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

sockaddrObj::sockaddrObj() noexcept
{
}

sockaddrObj::sockaddrObj(int af,
			 const std::string &p,
			 int n)
{
	switch (af) {
	case AF_UNIX:
		{
			struct sockaddr_un *pathptr=0;

			resize(sizeof(*pathptr)-sizeof(pathptr->sun_path)
			       +1+p.size());

			pathptr=(struct sockaddr_un *)&*begin();

			memset(pathptr, 0, size());

			pathptr->sun_family=AF_UNIX;
			strcpy(pathptr->sun_path, p.c_str());
		}
		break;
	case AF_INET:
		{
			struct sockaddr_in *sin;

			resize(sizeof(*sin));

			sin=(struct sockaddr_in *)&*begin();

			memset(sin, 0, size());

			sin->sin_family=AF_INET;
			sin->sin_port=htons(n);

			sin->sin_addr.s_addr=INADDR_ANY;
			errno=EINVAL;
			if (p.size() > 0 &&
			    inet_pton(AF_INET, p.c_str(), &sin->sin_addr) <= 0)
				throw SYSEXCEPTION(p);
		}
		break;
	case AF_INET6:
		{
			struct sockaddr_in6 *sin6;

			resize(sizeof(*sin6));

			sin6=(struct sockaddr_in6 *)&*begin();

			memset(sin6, 0, size());

			sin6->sin6_family=AF_INET6;
			sin6->sin6_port=htons(n);

			sin6->sin6_addr=in6addr_any;
			errno=EINVAL;
			if (p.size() > 0 &&
			    inet_pton(AF_INET6, p.c_str(), &sin6->sin6_addr)<=0)
				throw SYSEXCEPTION(p);
		}
		break;
	default:
		badfamily();
	}
}

sockaddrObj::sockaddrObj(const struct ::sockaddr *p)
{
	init_addr(p);
}

sockaddrObj::sockaddrObj(const struct ::sockaddr_storage *ptr)
{
	init_addr(reinterpret_cast<const struct ::sockaddr *>(ptr));
}

void sockaddrObj::init_addr(const struct ::sockaddr *p)
{
	switch (p->sa_family) {
	case AF_UNIX:
		insert(end(), (char *)p,
		       (char *)p+sizeof(struct sockaddr_un));
		break;
	case AF_INET:
		insert(end(), (char *)p,
		       (char *)p +sizeof(struct sockaddr_in));
		break;
	case AF_INET6:
		insert(end(), (char *)p,
		       (char *)p +sizeof(struct sockaddr_in6));
		break;
	default:
		badfamily();
	}
}

sockaddrObj::~sockaddrObj() noexcept
{
}

void sockaddrObj::badfamily() const
{
	throw EXCEPTION("Invalid or unknown network address family");
}

bool sockaddrObj::operator<(const sockaddrObj &o) const
{
	const struct ::sockaddr *sa=(const struct ::sockaddr *)&*begin();

	if (size() < sizeof(sa->sa_family))
		badfamily();

	const struct ::sockaddr *sa2=(const struct ::sockaddr *)&*o.begin();

	if (o.size() < sizeof(sa2->sa_family))
		badfamily();

	if (sa->sa_family != sa2->sa_family)
		return sa->sa_family < sa2->sa_family;

	// Can't do bytewise comparison due to padding in some families

	return operator std::string() < o.operator std::string();
}

bool sockaddrObj::operator==(const sockaddrObj &o) const
{
	// Can't do bytewise comparison due to padding in some families

	return operator std::string() == o.operator std::string();
}

int sockaddrObj::family() const
{
	const struct ::sockaddr *sa=(const struct ::sockaddr *)&*begin();

	if (size() < sizeof(sa->sa_family))
		badfamily();

	return sa->sa_family;
}

const char sockaddrObj::anonlocalsocket[]="[anonlocalsocket]";

#define FAMILY(type,name) const struct ::type *name=(const struct ::type *)&*begin()
#define FAMILY_NONCONST(type,name) struct ::type *name=(struct ::type *)&*begin()
sockaddrObj::operator std::string() const
{
	if (family() == AF_UNIX)
		return address();

	std::ostringstream o;

	o << address();

	int p=port();

	if (p)
		o << "/" << p;

	return o.str();
}

std::string sockaddrObj::address() const
{
	switch (family()) {
	case AF_UNIX:
		{
			FAMILY(sockaddr_un, saun);

			if ((size_t)((const char *)(saun->sun_path)-&*begin())
			    > size())
			{
			anonsocket:
				return anonlocalsocket;
			}

			const char *b=saun->sun_path;
			const char *e=(&*begin())+size();

			std::string s;

			if (b < e && *b == 0)
			{
				s += "[anon]:";
				++b; // Linux abstract namespace
			}

			s += std::string(b, e);

			//! Could include trailing nulls

			const char *p=s.c_str();

			if (!*p) goto anonsocket;

			return p;
		}
	case AF_INET:
		{
			FAMILY(sockaddr_in, sin);

			if ((size_t)((const char *)(&sin->sin_addr+1)-&*begin())
			    > size())
				badfamily();

			char buf[INET_ADDRSTRLEN];

			if (!inet_ntop(AF_INET, &sin->sin_addr,
				       buf, sizeof(buf)))
				throw SYSEXCEPTION("inet_ntop");
			return buf;
		}
	case AF_INET6:
		{
			FAMILY(sockaddr_in6, sin6);

			if ((size_t)((const char *)
				     (&sin6->sin6_addr+1)-&*begin())
			    > size())
				badfamily();

			char buf[INET6_ADDRSTRLEN];

			if (!inet_ntop(AF_INET6, &sin6->sin6_addr,
				       buf, sizeof(buf)))
				throw SYSEXCEPTION("inet_ntop");
			return buf;
		}
	}

	badfamily();
}

int sockaddrObj::port() const
{
	switch (family()) {
	case AF_UNIX:
		return 0;

	case AF_INET:
		{
			FAMILY(sockaddr_in, sin);

			if ((size_t)((const char *)(&sin->sin_addr+1)-&*begin())
			    > size())
				badfamily();

			return ntohs(sin->sin_port);
		}
	case AF_INET6:
		{
			FAMILY(sockaddr_in6, sin6);

			if ((size_t)((const char *)
				     (&sin6->sin6_addr+1)-&*begin())
			    > size())
				badfamily();

			return ntohs(sin6->sin6_port);
		}
	}

	badfamily();
}

void sockaddrObj::port(int portnum)
{
	switch (family()) {
	case AF_INET:
		{
			FAMILY_NONCONST(sockaddr_in, sin);

			if ((size_t)((const char *)(&sin->sin_addr+1)-&*begin())
			    > size())
				badfamily();

			sin->sin_port=htons(portnum);
		}
		break;
	case AF_INET6:
		{
			FAMILY_NONCONST(sockaddr_in6, sin6);

			if ((size_t)((const char *)
				     (&sin6->sin6_addr+1)-&*begin())
			    > size())
				badfamily();

			sin6->sin6_port=htons(portnum);
		}
		break;
	default:
		badfamily();
	}
}



bool sockaddrObj::is46(const sockaddrObj &o) const
{
	if (family() != AF_INET)
	{
		if (o.family() == AF_INET)
			return o.is46(*this);
		return false;
	}

	if (o.family() != AF_INET6)
		return false;

	FAMILY(sockaddr_in, sin);

	if ((size_t)((const char *)(&sin->sin_addr+1)-&*begin()) > size())
		badfamily();

	char buf[INET_ADDRSTRLEN+sizeof("::ffff:")];

	if (sin->sin_addr.s_addr == INADDR_ANY)
		strcpy(buf, "::");
	else
	{
		strcpy(buf, "::ffff:");

		if (!inet_ntop(AF_INET, &sin->sin_addr, buf+7,
			       sizeof(buf)-7))
			throw SYSEXCEPTION("inet_ntop");
	}

	struct ::sockaddr_in6 sin6=::sockaddr_in6();

	sin6.sin6_family=AF_INET6;
	sin6.sin6_port=sin->sin_port;

	errno=EINVAL;
	if (inet_pton(AF_INET6, buf, &sin6.sin6_addr) <= 0)
		throw SYSEXCEPTION(buf);

	if (o.size() < (const char *)(&sin6.sin6_addr+1)-(const char *)&sin6)
		return false;

	struct sockaddr_in6 sin6b;

	memcpy(&sin6b, &*o.begin(),
	       (const char *)(&sin6.sin6_addr+1)-(const char *)&sin6);

	return (memcmp(&sin6.sin6_addr, &sin6b.sin6_addr,
		       sizeof(sin6.sin6_addr)) == 0 &&
		sin6.sin6_port == sin6b.sin6_port);
}

#if 0
{
#endif
}
