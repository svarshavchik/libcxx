/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_eventfactoryfwd_H
#define x_eventfactoryfwd_H

#include <x/ptr.H>
#include <x/eventhandlerobj.H>

#include <iostream>

#include <x/namespace.h>


namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Event factory implementation

template<typename keyType, typename argType,
	 class Compare = std::less<keyType>,
	 class Allocator = std::allocator<std::pair<const keyType,
						    ptr<eventhandlerObj<argType> >
						    > > >
class eventfactoryObj;

#if 0
{
#endif
}
#endif
