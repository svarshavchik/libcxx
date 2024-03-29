/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_grp_H
#define x_grp_H

#include <x/namespace.h>
#include <x/exception.H>
#include <grp.h>
#include <vector>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A thread-safe wrapper for a \c struct \c group.

class group {

	//! The embeded group structure
	struct ::group grstruct;

	//! Buffer for strings
	std::vector<char> buffer;

public:
	//! Deleted copy constructor
	group(const group &)=delete;

	//! Deleted assignment operator
	group &operator=(const group &)=delete;

	//! getgrnam() equivalent

	//! Throws an exception if an error occured. If the groupname was not
	//! found, ->pw_name is a NULL ptr.

	group(const std::string &groupname);

	//! getgrgid() equivalent

	//! Throws an exception if userid not found.
	//!
	group(gid_t groupid);

	//! Destructor

	~group();

	//! The -> operator

	const struct ::group *operator->() noexcept { return &grstruct; }

	//! The * operator

	const struct ::group &operator*() noexcept { return grstruct; }

	//! Call getgroups(2), dedupe, supplement with getegid().

	static void getgroups(std::vector<gid_t> &groups);

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
