/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sighandlerobj_H
#define x_sighandlerobj_H

#include <x/obj.H>
#include <x/exception.H>
#include <x/logger.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Base class for signal handlers

class sighandlerObj : virtual public obj {

	//! The logger object.

	LOG_CLASS_SCOPE;

public:
	class priv;

	//! Constructor
	sighandlerObj();

	//! Destructor
	~sighandlerObj();

	//! Signal handler

	//! When this handler is installed for a particular signal, signal()
	//! gets invoked when the process receives the indicated signal.
	//! The same handler object may be installed for multiple signals.

	virtual void signal(int signum)=0;
};

#if 0
{
#endif
}
#endif
