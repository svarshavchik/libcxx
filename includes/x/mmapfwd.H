/*
** Copyright 2013-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_mmapfwd_H
#define x_mmapfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

template<typename typeObj> class mmapObj;

//! A memory-mapped object

//! \code
//! struct X;
//!
//! INSERT_LIBX_NAMESPACE::fd fdFile=INSERT_LIBX_NAMESPACE::fd::base::shm_open(filename, O_RDWR|O_CREAT, 0600);
//!
//! INSERT_LIBX_NAMESPACE::mmap<X> mapped=INSERT_LIBX_NAMESPACE::mmap<X>::create(fd, PROT_READ|PROT_WRITE);
//!
//! X *instance=mapped->object();
//!
//! mapped->msync();
//!
//! \endcode
//!
//! \c INSERT_LIBX_NAMESPACE::mmap is an interface to the mmap(2) system call.
//! The constructor maps the memory segment. When the last reference to this
//! reference-counted object goes out of scope and it gets destroyed, the
//! destructor call munmap().
//!
//! The template parameter is typically a POD class, but a real object could
//! possibly be used, if its constructor and destructor invocations are
//! carefully handled.
//!
//! The create() constructor takes the following arguments.
//!
//! - An optional \c void *, specifying the first argument to mmap(2), if not
//! present it defaults to \c nullptr; then either:
//!
//! - An open \ref fd "INSERT_LIBX_NAMESPACE::fd", the protection setting,
//! and three optional parameters: flags, offset, and length. This maps in the
//! opened file descriptor.
//!
//! - A length, the protection setting, and an optional flag value. This creates
//! a standalone mapped segment.
//!
//! These parameters get forwarded to mmap(2), mostly unchanged. The optional
//! flag value defaults to \c MAP_SHARED. For a file-based mapping the offset
//! value defaults to 0. For the file-based mapping, the default length value
//! of 0 is replaced with the current size of the file, as obtained by the
//! stat(2) system call.
//!
//! msync() calls the msync(2) system call.
//!
//! object() returns a pointer to the mapped memory segment, casted to a pointer
//! to the template type.

template<typename typeObj> using mmap=ref<mmapObj<typeObj>>;

//! A nullable pointer to a \ref mmap "memory mapped object".

template<typename typeObj> using mmapptr=ptr<mmapObj<typeObj>>;

//! A reference to a constant \ref mmap "memory mapped object".

template<typename typeObj> using const_mmap=const_ref<mmapObj<typeObj>>;

//! A nullable pointer reference to a constant \ref mmap "memory mapped object".

template<typename typeObj> using const_mmapptr=const_ptr<mmapObj<typeObj>>;

#if 0
{
#endif
}
#endif
