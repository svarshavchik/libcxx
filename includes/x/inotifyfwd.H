/*
** Copyright 2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_inotifyfwd_H
#define x_inotifyfwd_H

#include <cstdint>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class inotifyObj;
class exception;
class inotifyBase;

//! A reference to an inotify file descriptor.

//! \c inotify encapsulates a Linux inotify(7) file desciptor, that
//! monitors for changes to the contents of a file or a directory.
//!
//! \code
//! auto i=INSERT_LIBX_NAMESPACE::inotify::create();
//!
//! INSERT_LIBX_NAMESPACE::ref<INSERT_LIBX_NAMESPACE::obj> w=i->create(".", INSERT_LIBX_NAMESPACE::inotify_create | INSERT_LIBX_NAMESPACE::inotify_delete, []
//!              (uint32_t mask, uint32_t cookie, const char *name)
//!              {
//!                  // Do something
//!              });
//!
//! i->read();
//! \endcode
//!
//! The \c inotify object's create() function creates a new watch for the
//! given file or directory. The first parameter is the file or the directory
//! to watch, the second parameter specifies the events to watch for,
//! the third parameter is a lambda.
//!
//! \c create() returns a mcguffin for the installed watch. read() processes
//! the next inotify event, and invokes the appropriate lambda.
//! When the returned mcguffin goes out of scope and gets destroyed, the
//! watch gets terminated immediately.
//!
//! \c create()'s contract is that the lambda will no longer be invoked after
//! the mcguffin goes out of scope and gets destroyed.
//!
//! The lambda receives a mask of occured events, and the event cookie, as
//! described in inotify(7)'s documentation.
//!
//! \note
//! read() holds temporary references on mcguffins while their lambdas are
//! being invoked. If another thread drops the last reference it has to a
//! mcguffin while read() is in progress of invoking lambdas, it's possible
//! that the lambda still gets called, from its real last reference.
//! Attach a destructor callback to the mcguffin, and wait for it to be
//! actually destroyed, if needed.

typedef ref<inotifyObj> inotify;

//! A reference pointer to an event file descriptor

//! \see inotify

typedef ptr<inotifyObj> inotifyptr;

//! File was accessed.
extern const uint32_t inotify_access;

//! File attributes changed.
extern const uint32_t inotify_attrib;

//! File opened for writing was closed.
extern const uint32_t inotify_close_write;

//! File not opened for writing was closed.
extern const uint32_t inotify_close_nowrite;

//! File was created in the watched directory.
extern const uint32_t inotify_create;

//! File was deleted in the watched directory.
extern const uint32_t inotify_delete;

//! Watched directory was deleted.
extern const uint32_t inotify_delete_self;

//! File was modified.
extern const uint32_t inotify_modify;

//! Watched directory was moved.
extern const uint32_t inotify_move_self;

//! File was renamed from the watched directory.
extern const uint32_t inotify_moved_from;

//! File was renamed to the watched directory.
extern const uint32_t inotify_moved_to;

//! File was opened.
extern const uint32_t inotify_open;

//! Equivalent to inotify_moved_from | inotify_moved_to.
extern const uint32_t inotify_move;

//! Equivalent to inotify_close_write | inotify_close_nowrite.
extern const uint32_t inotify_close;

//! Don't derefence a symbolic link, if the pathname is one.
extern const uint32_t inotify_dont_follow;

//! Stop generating events after the specified pathname is unlinked.
extern const uint32_t inotify_excl_unlink;

//! Only watch the requested pathname if it's a directory
extern const uint32_t inotify_onlydir;

//! Reported event - all further events will be ignored
extern const uint32_t inotify_ignored;

//! Reported event - event is for a directory
extern const uint32_t inotify_isdir;

//! Reported event - filesystem with the pathname was unmounted
extern const uint32_t inotify_unmount;

#if 0
{
#endif
}

#endif
