/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_scalarnode_H
#define x_scalarnode_H

#include <x/namespace.h>
#include <x/yaml/scalarnodeobj.H>
#include <x/ref.H>
#include <x/ptr.H>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

//! A reference to a YAML scalar node object

//! \code
//! void dump(const INSERT_LIBX_NAMESPACE::yaml::scalarnode &scalar)
//! {
//!     std::string s=scalar->value;
//! }
//! \endcode
//!
//! This is a reference to a subclass of \ref node "YAML document node superclass"
//! that represents a scalar node in a YAML document. Its \c value member
//! gives the value of the scalar node.

typedef ref<scalarnodeObj> scalarnode;

//! A nullable pointer reference to a YAML scalar node object

typedef ptr<scalarnodeObj> scalarnodeptr;

//! A reference to a constant YAML scalar node object

typedef const_ref<scalarnodeObj> const_scalarnode;

//! A nullable pointer reference to a constant YAML scalar node object

typedef const_ptr<scalarnodeObj> const_scalarnodeptr;

#if 0
{
#endif
}
#endif
