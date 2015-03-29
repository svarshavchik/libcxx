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
//!
//! \see make_fdtimeoutconfig()

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

//! Helper class returned from make_fdtimeoutconfig()

template<typename lambda_type>
class fdtimeoutconfig_helper : public fdtimeoutconfig {

public:
	//! Lambda that's called to return timeout configuration

	mutable lambda_type lambda;

	//! Constructor
	fdtimeoutconfig_helper(lambda_type &&lambdaArg)
		: lambda(std::move(lambdaArg))
	{
	}

	//! Destructor
	~fdtimeoutconfig_helper() noexcept {}

	//! Return the timeout wrapper for a file descriptor
	fdbase operator()(const fd &fd) const override
	{
		return lambda(fd);
	}
};

//! Create an fdtimeoutconfig from a lambda.

//! Returns a subclass of \ref fdtimeoutconfig "fdtimeoutconfig" that uses
//! the supplied lamba.
template<typename lambda_type>
auto make_fdtimeoutconfig(//! Lambda that implements fdtimeoutconfig
			  lambda_type &&lambda)
	-> fdtimeoutconfig_helper<typename
				  std::remove_reference<lambda_type>::type>
{
	return fdtimeoutconfig_helper<typename
				      std::remove_reference<lambda_type>::type>
		(std::move(lambda));
}

#if 0
{
#endif
}
#endif
