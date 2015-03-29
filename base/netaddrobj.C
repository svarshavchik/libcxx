/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/netaddrobj.H"
#include "x/strtok.H"
#include "x/fd.H"
#include <x/exception.H>
#include <cstring>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>

#ifndef AI_IDN
#define AI_IDN 0
#endif

#ifndef AI_CANONIDN
#define AI_CANONIDN 0
#endif

#define DEFAULT_AI_FLAGS AI_ADDRCONFIG|AI_IDN|AI_CANONNAME|AI_CANONIDN

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

netaddrObj::netaddrObj() noexcept
	: flagsval(DEFAULT_AI_FLAGS), domain_hint(AF_UNSPEC),
	  type_hint(0),
	  protocol_hint(0)
{
}

netaddrObj::netaddrObj(const std::string &node, const std::string &service,
		       int type) noexcept
	: nodeval(node), serviceval(service),
	  flagsval(DEFAULT_AI_FLAGS), domain_hint(AF_UNSPEC), type_hint(type),
	  protocol_hint(0)
{
}

netaddrObj::netaddrObj(const std::string &node, int service,
		       int type) noexcept
	: nodeval(node),
	  flagsval(DEFAULT_AI_FLAGS), domain_hint(AF_UNSPEC), type_hint(type),
	  protocol_hint(0)
{
	std::ostringstream o;

	o << service;

	serviceval=o.str();
}

netaddrObj::netaddrObj(const std::string &node,
		       const std::list<int> &ports,
		       int type) noexcept
	: nodeval(node),
	  flagsval(DEFAULT_AI_FLAGS), domain_hint(AF_UNSPEC), type_hint(type),
	  protocol_hint(0)
{
	std::ostringstream o;
	const char *sep="";

	for (std::list<int>::const_iterator b=ports.begin(), e=ports.end();
	     b != e; ++b)
	{
		o << sep << *b;
		sep=",";
	}

	serviceval=o.str();
}

netaddrObj::netaddrObj(int type,
		       const std::string &address_arg,
		       const std::string &default_service_arg)

	: flagsval(DEFAULT_AI_FLAGS), domain_hint(AF_UNSPEC),
	  type_hint(type),
	  protocol_hint(0)
{
	std::string address(address_arg);
	std::string default_service(default_service_arg);

	size_t colon_pos=address.find(':');

	if (colon_pos != address.npos)
	{
		std::string method=address.substr(0, colon_pos);

		if (method == "file")
		{
			domain_hint=AF_UNIX;
			serviceval=address.substr(colon_pos+1);
			return;
		}

		if (method != "inet")
			throw EXCEPTION("Unknown address format: " + method);

		address=address.substr(colon_pos+1);
	}
	else
	{
		if (*address.c_str() == '/')
		{
			domain_hint=AF_UNIX;
			serviceval=address;
			return;
		}
	}

	size_t slash_pos=address.rfind('/');

	if (slash_pos != address.npos)
	{
		default_service=address.substr(slash_pos+1);
		address=address.substr(0, slash_pos);
	}

	nodeval=address;
	serviceval=default_service;
}

netaddrObj::~netaddrObj() noexcept
{
}

netaddr netaddrObj::service(//! Network port, as an integer
			    int port) noexcept
{
	std::ostringstream o;

	o << port;

	return service(o.str());
}

netaddrObj::operator netaddr::base::result() const
{
	return get_results(0);
}

netaddr::base::result
netaddrObj::get_results(int extra_flags) const
{
	netaddr::base::result
		new_res(netaddr::base::result::create());

	if (domain_hint == AF_UNIX)
	{
		new_res->canonname=serviceval;

		sockaddr sa(sockaddr::create());

		new_res->addrlist.push_back(netaddrResultObj::addrstruct(sa));

		netaddrResultObj::addrstruct &as=new_res->addrlist.back();

		as.domain=PF_UNIX;
		as.type=SOCK_STREAM;
		as.protocol=0;

		struct sockaddr_un *pathptr=0;

		sa->resize(sizeof(*pathptr)-sizeof(pathptr->sun_path)
			   +1+serviceval.size());

		pathptr=(struct sockaddr_un *)&*sa->begin();

		memset(pathptr, 0, sa->size());

		pathptr->sun_family=AF_UNIX;
		strcpy(pathptr->sun_path, serviceval.c_str());

		return new_res;
	}

	std::list<std::string> svc_list;

	strtok_str(serviceval, ", \t\r\n", svc_list);

	do
	{
		const char *node_cptr=nodeval.c_str();
		const char *service_cptr=0;
		struct ::addrinfo hints=::addrinfo();
		struct ::addrinfo *res;

		if (*node_cptr == 0)
			node_cptr=0;

		if (!svc_list.empty())
			service_cptr=svc_list.front().c_str();

		hints.ai_flags=flagsval | extra_flags;

		if ((hints.ai_flags & AI_PASSIVE) || node_cptr == NULL)
			hints.ai_flags &= ~(AI_CANONNAME|AI_CANONIDN);
		hints.ai_family=domain_hint;
		hints.ai_socktype=type_hint;
		hints.ai_protocol=protocol_hint;

#ifndef LIBCXX_DEBUG_GETADDRINFO
#define LIBCXX_DEBUG_GETADDRINFO(x) (x)
#endif

		int rc=getaddrinfo(LIBCXX_DEBUG_GETADDRINFO(node_cptr), service_cptr, &hints, &res);

		if (rc)
			throw EXCEPTION(gai_strerror(rc));

		try {

			struct ::addrinfo *p;

			if (res && res->ai_canonname)
				new_res->canonname=res->ai_canonname;

			for (p=res; p; p=p->ai_next)
			{
				sockaddr sa(sockaddr::create());

				new_res->addrlist
					.push_back(netaddrResultObj
						   ::addrstruct(sa));

				netaddrResultObj::addrstruct &as=
					new_res->addrlist.back();

				as.domain=p->ai_family;
				as.type=p->ai_socktype;
				as.protocol=p->ai_protocol;

				sa->resize(p->ai_addrlen);
				memcpy(&*sa->begin(), p->ai_addr,
				       p->ai_addrlen);
			}
		} catch (...)
		{
			freeaddrinfo(res);
			throw;
		}
		freeaddrinfo(res);

		if (!svc_list.empty())
			svc_list.pop_front();
	} while (!svc_list.empty());
	return new_res;
}

fd netaddrObj::connect() const
{
	return operator netaddr::base::result()->connect();
}

fd netaddrObj::connect(const fdtimeoutconfig &timeoutConfig)
{
	return (operator netaddr::base::result())->connect(timeoutConfig);
}

void netaddrObj::bind(std::list<fd> &fds, bool reuseaddr)
	const
{
	return get_results(AI_PASSIVE)->bind(fds, reuseaddr);
}


#if 0
{
#endif
}
