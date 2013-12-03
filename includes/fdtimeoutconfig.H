/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fdtimeoutconfig_H
#define x_fdtimeoutconfig_H

#include <x/fdtimeoutobj.H>
#include <x/ref.H>
#include <x/fdbasefwd.H>
#include <x/fdobj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

class fdObj;
class fdbaseObj;

//! Timeout configuration factory

//! Take an underlying file descriptor, and return a facade, presumably
//! \ref fdtimeout "fdtimeout" for reading or writing to the file descriptor
//! with the prescribed timeout mechanism.

class fdtimeoutconfig {

public:
	//! Install a timeout mechanism for a file descriptor

	//! The transport for reading or writing the file descriptor.

	virtual fdbase operator()(//! Original file descriptor
				  const fd &fd)
		const;

	class terminate_fd;
};

//! Attach a terminator file descriptor

//! Implement an \ref fdtimeoutconfig "fdtimeoutconfig" that adds a
//! terminator file descriptor to a primary file descriptor.

class fdtimeoutconfig::terminate_fd : public fdtimeoutconfig {

	//! The terminator file descriptor
	fd fdref;

public:
	//! Constructor

	terminate_fd(//! Terminator file descriptor to attach to every primary
		     const fd &fdrefArg);

	//! Destructor
	~terminate_fd() noexcept;

	//! Attach the terminator file descriptor.

	fdbase operator()(const fd &fdinst)
		const;
};

#if 0
{
#endif
}
#endif
