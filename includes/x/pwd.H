/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_pwd_H
#define x_pwd_H

#include <x/namespace.h>
#include <x/exception.H>
#include <pwd.h>
#include <vector>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A thread-safe wrapper for a \c struct \c passwd.

class passwd {

	//! The embeded passwd structure
	struct ::passwd pwstruct;

	//! Buffer for strings
	std::vector<char> buffer;

public:
	//! Deleted copy constructor

	passwd(const passwd &)=delete;

	//! Deleted assignment operator
	passwd &operator=(const passwd &)=delete;

	//! getpwnam() equivalent

	//! Throws an exception if an error occured. If the username was not
	//! found, ->pw_name is a NULL ptr.

	passwd(const std::string &username);

	//! getpwuid() equivalent

	//! Throws an exception if userid not found.
	//!
	passwd(uid_t userid);

	//! Destructor

	~passwd();

	//! The -> operator

	const struct ::passwd *operator->() const noexcept { return &pwstruct; }

	//! The * operator

	const struct ::passwd &operator*() const noexcept { return pwstruct; }

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
