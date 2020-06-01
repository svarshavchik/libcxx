/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_netaddrresult_H
#define x_netaddrresult_H

#include <x/netaddrfwd.H>
#include <x/netaddrresultobj.H>
#include <x/ptr.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Refer to this class as \c netaddr::base::result::base or \c netaddr::base::resultptr::base (or netaddrptr::...)

//! \internal
//! This class provides definitions for member types of
//! netaddrResultObj,
//! a reference to an object that holds
//! unresolved network addresses.

class netaddrResultBase : public ptrref_base {

public:
	//! A resolved address

	typedef netaddrResultObj::addrstruct addrstruct;

	//! A container for the list of socket addresses

	typedef netaddrResultObj::addrlist_t addrlist_t;
};

inline netaddr::base::result
netaddrResultObj::operator+(const netaddrResultObj &o)

{
	netaddr::base::result r=netaddr::base::result::create();

	(*r) += (*this);
	(*r) += o;
	return r;
}

#if 0
{
#endif
}
#endif
