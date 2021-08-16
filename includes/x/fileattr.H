/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_fileattr_H
#define x_fileattr_H

#include <x/exceptionfwd.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/basicattr.H>
#include <x/fileattrobj.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Retrieve a given file's status attributes. Retrieve and modify extended attributes.

//! The ::fileattr object provides methods for retrieving a given file's
//! status file (see the %stat(2) man page) and for retrieving and modifying
//! extended attributes.
//!
//! \code
//! fileattr myFileAttr(fileattr::create("filename", true));
//! \endcode
//!
//! The constructor specifies the filename whose status and extended attributes
//! are used. The second parameter is optional. It gets ignored unless
//! the filename refers to a symbolic link. If the filename is a symbolic link
//! and the second parameter is \c true (default), the symbolic link's
//! extended attributes get manipulated, if \c false the file that the symbolic
//! link points to has its extended attributes manipulated.
//! This is a reference to an object that implements basic_attr.
//!
//! \par File status
//!
//! \code
//!
//! auto stat_flags=myFileAttr->stat();
//!
//! if (stat_flags.st_uid == 0)
//! {
//!    // ....
//! \endcode
//!
//! The %stat() function returns the structure described in the %stat(2) man
//! page. An exception gets thrown if the %stat(2) system call fails.
//!
//! \code
//! auto stat_flags=myFileAttr->try_stat();
//!
//! if (stat_flags)
//! {
//!     if (stat_flags->st_uid == 0)
//!     {
//!        // ....
//! \endcode
//!
//! \c try_stat() does not throw an exception if %stat(2) fails.
//! try_stat() returns a \c std::optional. An undefined \c std::optional
//! indicates a failed \%stat(2) call.
//!
//! \par Extended file attributes
//!
//! \code
//!
//! std::set<std::string> attrList=myFileAttr->listattr();
//!
//! std::string value=myFileAttr->getattr("user.foo");
//!
//! myFileAttr->setattr("user.foo", "bar", XATTR_CREATE);
//!
//! myFileAttr->removeattr("user.foo");
//! \endcode
//!
//! \see basic_attr::stat()
//! \see basic_attr::listattr()
//! \see basic_attr::getattr()
//! \see basic_attr::setattr()
//! \see basic_attr::removeattr()

typedef ref<fileattrObj> fileattr;

//! A pointer to a file attribute object

//! \see fileattr

typedef ptr<fileattrObj> fileattrptr;

#if 0
{
#endif
}
#endif
