/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_servent_H
#define x_servent_H

#include <x/namespace.h>
#include <x/exception.H>
#include <netdb.h>
#include <vector>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A thread-safe wrapper for a \c struct \c servent.

class servent {

	//! The embeded servent structure
	struct ::servent servstruct;

	//! Buffer for strings
	std::vector<char> buffer;

public:
	//! Deleted copy constructor
	servent(const servent &)=delete;

	//! Deleted assignment operator
	servent &operator=(const servent &)=delete;

	//! getservbyname() equivalent

	//! Throws an exception if an error occured. If the service name was not
	//! found, ->s_name is a NULL ptr.

	servent(const std::string &servname,
		const std::string &proto);

	//! getservbyport() equivalent

	//! Throws an exception if an error occured. If the service name was not
	//! found, ->s_name is a NULL ptr.
	servent(int port,
		const std::string &proto);

	//! Destructor

	~servent();

	//! The -> operator

	const struct ::servent *operator->() const noexcept
	{ return &servstruct; }

	//! The * operator

	const struct ::servent &operator*() const noexcept
	{ return servstruct; }

private:
	//! Initialize the object

	//! \internal
	//!
	void init();
};

#if 0
{
#endif
}
#endif
