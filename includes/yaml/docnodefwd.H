/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_docnodefwd_H
#define x_docnodefwd_H

#include <x/namespace.h>
#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

	namespace yaml {
#if 0
	}
}
#endif

class docnodeBase;
class docnodeObj;

//! A node in a YAML document

typedef ref<docnodeObj, docnodeBase> docnode;

//! A nullable pointer reference to a node in a YAML document

typedef ptr<docnodeObj, docnodeBase> docnodeptr;

//! A constant node in a YAML document

typedef const_ref<docnodeObj, docnodeBase> const_docnode;

//! A nullable pointer reference to a const node in a YAML document

typedef const_ptr<docnodeObj, docnodeBase> const_docnodeptr;

#if 0
{
	{
#endif
	}
}
#endif
