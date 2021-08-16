/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/ 
#ifndef x_yaml_newscalarnodefwd_H
#define x_yaml_newscalarnodefwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE::yaml {
#if 0
}
#endif

class newscalarnodeObj;

//! A reference to a node in a new YAML document written by writer::write().

//! \code
//! return INSERT_LIBX_NAMESPACE::newscalarnode::create("apple");
//! \endcode
//!
//! At the very end of all the \ref newsequence "sequences" and
//! \ref newmapping "mappings", all that's left are scalar values
//! (and an occasional \ref newaliasnode "alias"), that
//! form the YAML document that's written by
//! \ref writer "writer::write"().

typedef ref<newscalarnodeObj> newscalarnode;

//! A nullable pointer reference to a \ref newscalarnode "newscalarnode".

typedef ptr<newscalarnodeObj> newscalarnodeptr;

#if 0
{
#endif
}

#endif
