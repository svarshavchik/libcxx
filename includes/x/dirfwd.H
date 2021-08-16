/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_dirfwd_H
#define x_dirfwd_H

#include <x/ref.H>
#include <x/ptr.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class dirObj;
class dirBase;

//! A directory whose contents will be read.

//! This is a reference to an object that holds the name of a directory
//! which gets opened to read its contents. This object's lifetime is
//! rather short:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::dir etc=INSERT_LIBX_NAMESPACE::dir::create("/etc");
//!
//! dir::base::iterator b=etc->begin(), e=etc->end();
//! \endcode
//!
//! dir::base::iterator is an input %iterator that iterates through the contents
//! of the directory. The original dir object is no longer required after
//! begin() and end() get invoked. begin() is the actual method that opens
//! the directory.
//!
//! By default, the "." and ".." directory entries are skipped over in the
//! iterated sequence. A second optional parameter to create() of \c true
//! includes the "." andthe ".." directory entry in the returned sequence.

typedef ref<dirObj, dirBase> dir;

//! A nullable pointer reference to a \ref dir "dir".

typedef ptr<dirObj, dirBase> dirptr;

//! A reference to a constant \ref dir "dir".

typedef const_ref<dirObj, dirBase> const_dir;

//! A nullable pointer reference to a constant \ref dir "dir".

typedef const_ptr<dirObj, dirBase> const_dirptr;

#if 0
{
#endif
}
#endif
