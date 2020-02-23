/*
** Copyright 2015-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ltdlfwd_H
#define x_ltdlfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class ltdlObj;
class ltdlBase;

//! A loaded shared library

//! \code
//! INSERT_LIBX_NAMESPACE::ltdl shlib=INSERT_LIBX_NAMESPACE::ltdl::create("libmodule");
//! \endcode
//!
//! This is a wrapper for loading shared libraries via libtool's libltdl.
//! The specified shared library gets loaded. An exception gets thrown if
//! the library could not be loaded.
//!
//! This is a \ref ref "reference to a reference-counted object". The shared
//! library gets unloaded after all references to the object go out of scope,
//! and it gets destroyed.
//!
//! \code
//! shlib->makeresident();
//!
//! bool flag=shlib->isresident();
//! \endcode
//!
//! makeresident() marks the shared library as resident, the shared library
//! does not get unloaded after the shared library object gets destroyed.
//! isresident() returns an indication whether the object is loaded.
//!
//! \code
//! auto sym=shlib->sym<int (*)()>("function");
//! \endcode
//!
//! Returns the address of the symbol in the loaded shared library, or a
//! nullptr if no such symbol exists, type-casted to the template class
//! parameter.
//!
//! \note
//! C++ symbols need to be mangled.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::ltdl::base::options opts=INSERT_LIBX_NAMESPACE::ltdl::base::options();
//!
//! opts->set_ext();
//!
//! opts->set_global();
//! opts->set_local();
//! opts->set_resident();
//! opts->set_preload();
//!
//! INSERT_LIBX_NAMESPACE::ltdl shlib=INSERT_LIBX_NAMESPACE::ltdl::create("libmodule", opts);
//! \endcode
//!
//! An \c INSERT_LIBX_NAMESPACE::ltdl::base::options optional second parameter
//! specifies non-default options for loading the indicated shared library:
//!
//! - set_ext(): automatically append system-specified extension to the
//! specified filename (this is enabled by default if the \c options object
//! is not specified as the second parameter to create(), but needs to be
//! specified explicitly if it's desired).
//!
//! - set_local(): symbols from the shared library should not be exported
//! to any other shared libraries to be loaded.
//!
//! - set_global(): symbols from the shared library should not be exported
//! to any other shared libraries to be loaded.
//!
//! - set_resident(): the shared library should be made resident, and not
//! unloaded, after its handle goes out of scope, and gets destroyed.
//!
//! - set_preload(): open the shared library only if its already been preloaded
//! (an exceptions gets thrown, as usual, if the shared library could not
//! be opened).
//!
//! \note
//! See libtool's libltdl library for more information.

typedef ref<ltdlObj, ltdlBase> ltdl;

//! A nullable pointer reference to a \ref ltdl "loaded shared library".
typedef ptr<ltdlObj, ltdlBase> ltdlptr;

//! A reference to a constant \ref ltdl "loaded shared library".
typedef const_ref<ltdlObj, ltdlBase> const_ltdl;

//! A nullable pointer reference to a constant \ref ltdl "loaded shared library".
typedef const_ptr<ltdlObj, ltdlBase> const_ltdlptr;

#if 0
{
#endif
}
#endif
