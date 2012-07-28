/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_filestat_H
#define x_filestat_H


#include <x/ptr.H>
#include <x/filestatobj.H>

#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! File status

//! This is a reference to an object that wraps the \c %stat structure
//! that's populated by one of the %stat(2) man page functions.
//!
//! This reference is returned by the
//! \link basic_attr::stat() stat() \endlink function, which is accessible
//! from the ::fileattr and ::fd objects.
//! \see ::fileattr
//! \see ::fd

typedef ref<filestatObj> filestat;

//! A nullable reference to a \c %stat structure.

//! \see filestat

typedef ptr<filestatObj> filestatptr;

#if 0
{
#endif
}
#endif
