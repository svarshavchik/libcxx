/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_netif_H
#define x_netif_H

#include <x/netifobj.H>
#include <x/exception.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class netifBase;

//! A reference to a network interface metadata object

typedef ref<netifObj, netifBase> netif;

//! A nullable pointer to a network interface metadata object.

typedef ptr<netifObj, netifBase> netifptr;

//! Refer to this class as \c netif::base or \c netifptr::base

class netifBase : public ptrref_base {

public:

	//! Enumerate all interfaces

	static void enumerate(//! Enumerated interfaces placed here
			      std::vector<netif> &interfaces);

	//! An address and a netmask

	//! This type is referenced as netif::ifaddr.
	//!

	typedef netifObj::ifaddr ifaddr;
};

#if 0
{
#endif
}
#endif
