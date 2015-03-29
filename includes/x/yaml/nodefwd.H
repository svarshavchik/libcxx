/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_nodefwd_H
#define x_nodefwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

class nodeObj;

//! A reference to a YAML document node superclass.

//! This is a \ref ref "reference" to a reference-counted object that
//! represents one node in a YAML document. The referenced object,
//! \ref nodeObj "INSERT_LIBX_NAMESPACE::yaml::nodeObj" is a superclass of
//! all possible node types in a YAML document. Use its \c nodetype
//! member to determine what kind of a node it is.
//!
//! - \c YAML_SCALAR_NODE: this reference is convertible to a
//! \ref scalarnode "INSERT_LIBX_NAMESPACE::yaml::scalarnode".
//!
//! - \c YAML_SEQUENCE_NODE: this reference is convertible to a
//! \ref sequencenode "INSERT_LIBX_NAMESPACE::yaml::sequencenode".
//!
//! - \c YAML_MAPPING_NODE: this reference is convertible to a
//! \ref mappingnode "INSERT_LIBX_NAMESPACE::yaml::mappingnode".
//!
//! - \c YAML_NO_NODE: this reference is not convertible to anything. This
//! gets returned for the root node of an empty YAML document.
//!
//! \code
//! void dosomething(const INSERT_LIBX_NAMESPACE::yaml::node &n)
//! {
//!     std::cout << "Tag: " << n->tag << std::endl;
//!
//!     switch (n->nodetype) {
//!     case YAML_SCALAR_NODE:
//!         dosomething(INSERT_LIBX_NAMESPACE::yaml::scalarnode(n));
//!         break;
//!     case YAML_SEQUENCE_NODE:
//!         dosomething(INSERT_LIBX_NAMESPACE::yaml::sequencenode(n));
//!         break;
//!     case YAML_MAPPING_NODE:
//!         dosomething(INSERT_LIBX_NAMESPACE::yaml::sequencenode(n));
//!         break;
//!     };
//! }
//! \endcode
//!
//! The \c tag member is a string that specifies the node's YAML tag.

typedef ref<nodeObj> node;

//! A nullable pointer reference to a \ref node "YAML node" object

typedef ptr<nodeObj> nodeptr;

//! A reference to a constant \ref node "YAML node" object

typedef const_ref<nodeObj> const_node;

//! A nullable pointer reference to a constant \ref node "YAML node" object

typedef const_ptr<nodeObj> const_nodeptr;

#if 0
{
	{
#endif
	}
}
#endif
