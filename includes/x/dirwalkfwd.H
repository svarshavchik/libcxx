/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_dirwalkfwd_H
#define x_dirwalk_H

#include <x/ref.H>
#include <x/ptr.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class dirwalkObj;
class dirwalkBase;

//! A directory whose contents will be recursively read

//! This is a version of dir that recursively reads the contents of the
//! directory and all of its subdirectories.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::dirwalk etc=INSERT_LIBX_NAMESPACE::dirwalk::create("/etc");
//!
//! INSERT_LIBX_NAMESPACE::dirwalk::base::iterator b=etc->begin(), e=etc->end();
//! \endcode
//!
//! INSERT_LIBX_NAMESPACE::dirwalk::base::iterator is an input %iterator
//! that iterates through the contents
//! of the directory, including all of its subdirectories.
//! The original dirwalk object is no longer required after
//! begin() and end() get invoked. begin() is the actual method that opens
//! the directory hierarchy.
//!
//! The %iterator returns
//! INSERT_LIBX_NAMESPACE::dir::base::entry objects, same
//! as INSERT_LIBX_NAMESPACE::dir::base::iterator.
//! The directory hierarchy is iterated in prefix order. That is, the %iterator
//! returns a INSERT_LIBX_NAMESPACE::dir::entry for a subdirectory before
//! returning entries for the
//! contents of the subdirectory. Specify an optional second argument of
//! \c true to iterate in suffix order, where the
//! INSERT_LIBX_NAMESPACE::dir::entry of a subdirectory
//! gets returned only after returning entries for the subdirectory's contents.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::dirwalk etc=INSERT_LIBX_NAMESPACE::dirwalk::create("/etc", true);
//!
//! INSERT_LIBX_NAMESPACE::dirwalk::base::iterator b=etc->begin(), e=etc->end();
//! \endcode
//!

typedef ref<dirwalkObj, dirwalkBase> dirwalk;

//! A nullable pointer reference to a \ref dir "dir".

typedef ptr<dirwalkObj, dirwalkBase> dirwalkptr;

#if 0
{
#endif
}
#endif
