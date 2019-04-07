/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_timer_H
#define x_timer_H

#include <x/namespace.h>
#include <x/threads/timerfwd.H>
#include <x/threads/timerobj.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Base class for \ref timer "INSERT_LIBX_NAMESPACE::timer" and \ref timerptr "INSERT_LIBX_NAMESPACE::timerptr"

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::timer::base or \c INSERT_LIBX_NAMESPACE::timerptr::base
//!

class timerbaseObj : public ptrref_base {

public:

	//! Return a shared, global, timer thread

	static timer global();

	//! The underlying system clock

	typedef timerObj::clock_t clock_t;

	//! The underlying system clock

	typedef timerObj::time_point_t time_point_t;

	//! The underlying system clock

	typedef timerObj::duration_t duration_t;

	//! Duration property type.

	//! See \ref timer "INSERT_LIBX_NAMESPACE::timer"
	//!
	typedef timerObj::repeatinfo duration_property_t;
};

#if 0
{
#endif
}
#endif
