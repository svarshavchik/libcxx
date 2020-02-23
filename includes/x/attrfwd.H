/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_attrfwd_H
#define x_attrfwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

class basic_attr;

//! Extended attribute reference.

//! This is a reference to an object that implements the basic_attr
//! interface. The available methods are:
//!
//! \code
//! attr myAttr;
//!
//! std::set<std::string> attrList=myAttr->listattr();
//!
//! std::string value=myAttr->getattr("user.foo");
//!
//! myAttr->setattr("user.foo", "bar", XATTR_CREATE);
//!
//! myAttr->removeattr("user.foo");
//! \endcode
//!
//! \see basic_attr::listattr()
//! \see basic_attr::getattr()
//! \see basic_attr::setattr()
//! \see basic_attr::removeattr()
//!
//! Note that this type does not define where the extended attributes come from.
//! To retrieve attributes from a named file, instantiate a
//! \link ::fileattr fileattr\endlink.
//! The extended attributes may also be retrieved from an open file
//! descriptor. fdObj implements the basic_attr interface, so fd (which is
//! a reference to fdObj) may be used in contexts that use
//! \link ::attr attr\endlink.

typedef ref<basic_attr> attr;

//! A nullable pointer to an attribute

typedef ptr<basic_attr> attrptr;

#if 0
{
#endif
}
#endif
