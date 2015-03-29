/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef mutex_H
#define mutex_H

#include <x/ref.H>
#include <x/mutexobj.H>
#include <x/mutexfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::mutex::base or \c INSERT_LIBX_NAMESPACE::mutexptr::base

class mutexBase : public ptrrefBase {

public:

	//! A convenience typedef

	typedef mutexObj::mlock mlock;

	//! A convenience typedef

	typedef mutexObj::mlockptr mlockptr;

};

#if 0
{
#endif
}
#endif
