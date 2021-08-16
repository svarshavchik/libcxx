/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_globlock_H
#define x_globlock_H

#include <sys/types.h>
#include <unistd.h>
#include <x/exception.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! An internal global lock

//! \internal

class globlock {

 public:
	//! Constructor
	globlock() LIBCXX_HIDDEN;

	//! Destructor
	~globlock() LIBCXX_HIDDEN;
};

//! Execute a fork() under a global lock.

//! A fork(2) call wrapped in a global lock. Use this fork() instead of
//! fork(2). The only difference in semantics that a return code of -1 throws
//! an exception automatically.

pid_t fork();


#if 0
{
#endif
}
#endif
