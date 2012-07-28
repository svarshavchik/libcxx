/*
** Copyright 2012 Double Precision, Inc.
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
//! the filename refers to a symbolic links. If the filename is a symbolic link
//! and the second parameter is \c true (default), the symbolic link's
//! extended attributes get manipulated, if \c false the file that the symbolic
//! link points to has its extended attributes manipulated.
//! This is a reference to an object that implements basic_attr.
//!
//! \par File status
//!
//! \code
//!
//! filestat stat_flags=myFileAttr->stat();
//!
//! if (stat_flags->st_uid == 0)
//! {
//!    // ....
//! \endcode
//!
//! The %stat() function retrives a reference-counted object that holds
//! the fields described in the %stat(2) man page.
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
