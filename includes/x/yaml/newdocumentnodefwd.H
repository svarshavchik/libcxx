/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newdocumentnodefwd_H
#define x_yaml_newdocumentnodefwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

class newdocumentnodeObj;

//! A reference to a new YAML document written by writer::write().

//! The reference-counted class newdocumentnodeObj defines a virtual
//! method root() that returns the root node of a new YAML document.
//! Derive a subclass of newdocumentnodeObj and implement root() that
//! returns a \ref newnode "newnode", or use
//! \ref make_newdocumentnode "make_newdocumentnode"() which constructs one
//! from a functor.

typedef ref<newdocumentnodeObj> newdocumentnode;

//! A nullable pointer reference to a \ref newdocumentnode "newdocumentnode".

typedef ptr<newdocumentnodeObj> newdocumentnodeptr;

//! A reference to a constant YAML document written by writer::write().

typedef const_ref<newdocumentnodeObj> const_newdocumentnode;

//! A nullable pointer reference to a constant YAML document written by writer::write().

typedef const_ptr<newdocumentnodeObj> const_newdocumentnodeptr;

template<typename functor_type>
newdocumentnode make_newdocumentnode(functor_type &&functor);

#if 0
{
#endif
}

#endif
