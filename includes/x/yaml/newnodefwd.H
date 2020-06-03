/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newnodefwd_H
#define x_yaml_newnodefwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

class newnodeObj;

//! A reference to a node in a new YAML document written by writer::write().

//! A YAML document that's written by writer::write() is represented as a
//! hierarchy of \c newnodes.
//! A \c newnode is \ref ref "reference" to a superclass
//! of one of several possible nodes:
//!
//! - \ref newscalarnode "newscalarnode": a scalar value.
//!
//! - A YAML sequence created by \ref newsequence "newsequence::create"().
//!
//! - A YAML mapping created by \ref newmapping "newmapping::create"().
//!
//! - \ref newaliasnode "newaliasnode": a YAML alias reference.
//!
//! - Whatever one of the above gets returned by
//! \ref make_newnode "make_newnode"()s functor.
//!
//! \ref newdocumentnode "newdocumentnode"'s \c root() method typically
//! returns the YAML document's top level sequence or mapping.
//! \ref writer "writer::write"() writes out the entire YAML document starting
//! with its top level node, and recursively writing out each node in the
//! hierarchical YAML document.
//!
//! The functors get invoked by writer::write() as it works its way writing
//! out the YAML document. Once some functor representing a YAML node
//! in the document's hierarchy returns a mapping or a sequence, after it
//! gets written out it is no longer needed, and all the reference-counted
//! objects that represent the sequence's or the mapping's values go out of
//! scope and get destroyed, so the written YAML document does not typically
//! get constructed, in its entirety, in memory.
//!
//! 
typedef ref<newnodeObj> newnode;

//! A nullable pointer reference to a \ref newnode "newnode".

typedef ptr<newnodeObj> newnodeptr;

//! A reference to a constant \ref newnode "newnode".

typedef const_ref<newnodeObj> const_newnode;

//! A nullable pointer reference to a constant \ref newnode "newnode".

typedef const_ptr<newnodeObj> const_newnodeptr;

template<typename functor_type> class newnode_functorObj;

#if 0
{
#endif
}

#endif
