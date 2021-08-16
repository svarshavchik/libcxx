/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newaliasnodefwd_H
#define x_yaml_newaliasnodefwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

class newaliasnodeObj;

//! A reference to a node in a new YAML document processed by writer::write().

//! \code
//!
//! return INSERT_LIBX_NAMESPACE::yaml::newsequence<>::create([]
//!     (INSERT_LIBX_NAMESPACE::yaml::newsequence<> &info)
//!     {
//!         info.anchor="fruits";
//!         info.container.push_back(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("apple"));
//!         info.container.push_back(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("pear"));
//!         info.container.push_back(INSERT_LIBX_NAMESPACE::yaml::newscalarnode::create("cherry"));
//!      });
//!
//! // ...
//!
//! return INSERT_LIBX_NAMESPACE::newaliasnode::create("fruits");
//! \endcode
//!
//! Returns an alias referring to some other node in the written document
//! that declares a matching anchor.

typedef ref<newaliasnodeObj> newaliasnode;

//! A nullable pointer reference to a \ref newaliasnode "newaliasnode".

typedef ptr<newaliasnodeObj> newaliasnodeptr;

#if 0
{
#endif
}

#endif
