/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_signalfd_H
#define x_signalfd_H

#include <x/signalfdfwd.H>
#include <x/signalfdobj.H>
#include <x/ptr.H>
#include <x/ref.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Base class for \ref signalfd signal file descriptors.

//! The base class defines the getsignal_t convenience typedef

class signalfdBase : public ptrref_base {

public:
	//! Convenience typedef
	typedef signalfdObj::getsignal_t getsignal_t;
};

#if 0
{
#endif
}

#endif
