/*
** Copyright 2015-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mmapfilefwd_H
#define x_mmapfilefwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class mmapfileObj;

//! A memory-mapped file

//! \code
//! INSERT_LIBX_NAMESPACE::fd fdFile=INSERT_LIBX_NAMESPACE::fd::base::shm_open(filename, O_RDWR|O_CREAT, 0600);
//!
//! INSERT_LIBX_NAMESPACE::mmapfile mapped=INSERT_LIBX_NAMESPACE::mmapfile::create(fd, PROT_READ|PROT_WRITE);
//!
//! size_t size=mapped->size();
//!
//! char *ptr=mmaped->buffer();
//!
//! mapped->msync();
//! \endcode
//!
//! \c INSERT_LIBX_NAMESPACE::mmapfile inherits from
//! \ref mmap "INSERT_LIBX_NAMESPACE::mmap" and offers a simplified interface
//! for memory-mapping a file.
//!
//! The create() constructor takes the following arguments.
//!
//! - An open \ref fd "INSERT_LIBX_NAMESPACE::fd", and the protection setting.
//!
//! buffer() returns a pointer to the mapped memory segment.
//! size() returns the size of the mapped file.

typedef ref<mmapfileObj> mmapfile;

//! A nullable pointer to a \ref mmapfile "memory mapped file".

typedef ptr<mmapfileObj> mmapfileptr;

//! A reference to a constant \ref mmapfile "memory mapped file".

typedef const_ref<mmapfileObj> const_mmapfile;

//! A nullable pointer reference to a constant \ref mmapfile "memory mapped file".

typedef const_ptr<mmapfileObj> const_mmapfileptr;

#if 0
{
#endif
}
#endif
