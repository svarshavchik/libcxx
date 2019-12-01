/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mlockobj_H
#define x_mlockobj_H

#include <x/obj.H>
#include <x/mutexfwd.H>
#include <x/mlockfwd.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class condObj;

//! Mutex object lock

class mlockObj : virtual public obj {

	//! My locked mutex

	mutex m;

	//! Default constructor for an unlocked lock

public:

	//! Constructor
	mlockObj(const mutex &m) LIBCXX_HIDDEN;

	//! Destructor
	~mlockObj();

	friend class condObj;
};

#if 0
{
#endif
}
#endif
