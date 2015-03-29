/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_filestatobj_H
#define x_filestatobj_H


#include <sys/types.h>
#include <sys/stat.h>
#include <x/obj.H>

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! A wrapper for a \c %stat structure

//! This is a reference-counted object that wraps the \c %stat structure
//! returned by one of the system calls described in the %stat(2) man page.
//!
//! A reference to this object is returned by
//! \link ::fileattr fileattr->stat() \endlink and
//! \link ::fd fd->stat() \endlink functions.

class filestatObj : virtual public obj, public ::stat {

public:
	//! The default constructor.
	filestatObj() noexcept;

	//! The default destructor.
	~filestatObj() noexcept;
};

#if 0
{
#endif
}
#endif
