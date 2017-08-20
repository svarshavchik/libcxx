/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_dirwalk_H
#define x_dirwalk_H

#include <x/dirwalkfwd.H>
#include <x/dirwalkobj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Base class of directory walk objects.

//! Refer to this base class as \c INSERT_LIBX_NAMESPACE::dirwalk::base or \c INSERT_LIBX_NAMESPACE::dirwalkptr::base

class dirwalkBase : public ptrref_base {

public:

	//! A convenience typedef

	//! Refer to the iterator as \c INSERT_LIBX_NAMESPACE::dirwalk::base::iterator or \c INSERT_LIBX_NAMESPACE::dirwalkptr::base::iterator
	//!
	typedef dirwalkObj::iterator iterator;

};

#if 0
{
#endif
}
#endif
