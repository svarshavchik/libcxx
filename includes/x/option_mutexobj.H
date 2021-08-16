/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_option_mutexobj_H
#define x_option_mutexobj_H

#include <x/namespace.h>
#include <x/ptr.H>
#include <x/obj.H>
#include <x/option_mutexfwd.H>

namespace LIBCXX_NAMESPACE::option {
#if 0
};
#endif

//! Maintain a flag that a mutually exclusive %option are set

//! \internal
//! This object stores a single flag that's set when any mutually-exclusive
//! %option is specified.

class mutexObj : virtual public obj {

public:
	//! Default constructor
	mutexObj() noexcept;

	//! Default destructor
	~mutexObj();

	//! A mutually-exclude %option is set

	bool value;

	//! Add a value to this mutually-exclusive %option group

	mutexObj &add(const ref<valuebaseObj> &valueRef) noexcept;
};

#if 0
{
#endif
}
#endif
