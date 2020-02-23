/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_opendirobj_H
#define x_opendirobj_H

#include <x/obj.H>
#include <sys/types.h>
#include <dirent.h>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Object that holds a currently open directory.

//! This is a basic wrapper for the C library's DIR object.
//!

class opendirObj : virtual public obj {

public:
	//! The open \c DIR pointer.

	DIR *dirp;

	//! The directory name.

	std::string dirname;

	//! The default constructor.

	opendirObj() noexcept;

	//! The default destructor.
	~opendirObj();
};

#if 0
{
#endif
}
#endif
