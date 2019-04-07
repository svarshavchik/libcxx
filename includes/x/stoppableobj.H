/*
** Copyright 2016-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_stoppableobj_H
#define x_stoppableobj_H

#include <x/obj.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! An interface for something that's stoppable

//! An interface for a class that implements a method called stop().
//!
//! \ref stoppableBase::group() "stoppable::base::group()" installs
//! destructor callbacks for a sequence of objects. When
//! any one of those objects is destroyed, stop() gets invoked on the
//! next object in the sequence. The end result is that when any one in the
//! sequence of objects gets destroyed, all the others gets destroyed too.
//! [MUTUAL]

class stoppableObj : virtual public obj {

public:
	//! Constructor
	stoppableObj();

	//! Destructor
	~stoppableObj();

	//! Request this thread to terminate itself

	virtual void stop()=0;
};

#if 0
{
#endif
}
#endif
