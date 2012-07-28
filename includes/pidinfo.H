/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_pidinfo_H
#define x_pidinfo_H

#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif


//! Information about a process.

//! Pass a process id to the constructor (defaults to this process).
//!
//! The constructor throws an exception if the invoking process
//! is running in a chroot jail, and the process ID is not available.
//!
//! Additionally, on Linux, the constructor throws an exception for a process
//! id that's running under a different userid.
//!
//! Getting the pathname of a process running
//! under a different ID requires cooperating
//! through portmap.
//!
//! \see httportmap

class pidinfo {

public:

	//! The absolute pathname of the executable that started the process.
	std::string exe;

	//! The time that the executable was started, as an opaque string

	//! This should be treated as an opaque blob, with the only defined
	//! operation being a comparison for equality or inequality.
	std::string start_time;

	//! Constructor
	pidinfo(//! Return this process's information
		pid_t p=getpid());
	~pidinfo();
};

#if 0
{
#endif
}
#endif
